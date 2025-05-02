#include "surfacechartentity.h"

namespace A3D {

SurfaceChartEntity::SurfaceChartEntity(Entity* parent)
    : Entity(parent),
      m_enumStripThicknessX(0.8f),
      m_enumStripThicknessZ(0.8f) {

	A3D::Model* m = new A3D::Model(this);
    setModel(m);

	{
        A3D::Group* meshGroup = m->getOrAddGroup("Chart");
		meshGroup->setMaterial(A3D::Material::standardMaterial(A3D::Material::ColoredSurfaceMaterial));
		meshGroup->setMaterialProperties(new MaterialProperties);
	}

    for(std::size_t j = 0; j < AXIS_COUNT; ++j) {
        for(std::size_t i = 0; i < CHAXIND_COUNT; ++i) {
            A3D::Group* group      = m->getOrAddGroup(QStringLiteral("Indicators %1 for axis %2").arg(QString::number(i + 1), QString::number(j + 1)));
            LineGroup* lineGroup   = new LineGroup();
            m_indicatorLines[j][i] = lineGroup;
            group->setLineGroup(lineGroup);

            switch(i) {
            case CHAXIND_MAJOR_INDICATOR:
                lineGroup->setThickness(0.01f);
                break;
            case CHAXIND_MINOR_INDICATOR:
                lineGroup->setThickness(0.003f);
                break;
            default:
                break;
            }
        }
    }

	{
        A3D::Group* group = m->getOrAddGroup("Marker");
        group->setLineGroup(m_markerLineGroup = new LineGroup());
        m_markerLineGroup->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
        m_markerLineGroup->setThickness(0.01f);
	}
}

void SurfaceChartEntity::setChart(MapChart3D map) {
    m_mapChart = std::move(map);
    updateSurfaceMesh();
    updateIndicatorLines(AXIS_X);
    updateIndicatorLines(AXIS_Y);
    updateIndicatorLines(AXIS_Z);
    updateIndicatorLabels(AXIS_X);
    updateIndicatorLabels(AXIS_Y);
    updateIndicatorLabels(AXIS_Z);
}

MapChart3D const& SurfaceChartEntity::mapChart() const {
    return m_mapChart;
}

void SurfaceChartEntity::setEnumStripThickness(float xThickness, float zThickness) {
    m_enumStripThicknessX = xThickness;
    m_enumStripThicknessZ = zThickness;

    if(m_mapChart.isValid()) {
        updateSurfaceMesh();
    }
}

float SurfaceChartEntity::enumStripThicknessX() const {
    return m_enumStripThicknessX;
}

float SurfaceChartEntity::enumStripThicknessZ() const {
    return m_enumStripThicknessZ;
}

void SurfaceChartEntity::setLabelDistances(QVector3D distance_x_y_z) {
    m_labelDistances = distance_x_y_z;

    if(m_mapChart.isValid()) {
        updateIndicatorLabels(AXIS_X);
        updateIndicatorLabels(AXIS_Y);
        updateIndicatorLabels(AXIS_Z);
    }
}

QVector3D SurfaceChartEntity::labelDistances() const {
    return m_labelDistances;
}

static void
getVertexFromCoordinate(A3D::Mesh::Vertex& vx, std::vector<float> const& xAxis, std::vector<float> const& zAxis, std::vector<float> const& yData, std::size_t x, std::size_t z) {
    vx.Position3D     = QVector3D(xAxis[x], yData[x + (z * xAxis.size())], zAxis[z]);
    vx.TextureCoord2D = QVector2D(vx.Position3D.x(), vx.Position3D.z());
}

static void generateNormal(A3D::Mesh::Vertex& triangleVertexA, A3D::Mesh::Vertex& triangleVertexB, A3D::Mesh::Vertex& triangleVertexC) {
    triangleVertexA.Normal3D = triangleVertexB.Normal3D = triangleVertexC.Normal3D
        = QVector3D::normal(triangleVertexA.Position3D, triangleVertexB.Position3D, triangleVertexC.Position3D);
}

void SurfaceChartEntity::updateSurfaceMesh() {
    if(!model())
        return;

    Group* chartGroup = model()->getOrAddGroup("Chart");
    Mesh* chartMesh   = chartGroup->mesh();

    if(!m_mapChart.isValid()) {
        if(chartMesh) {
            chartMesh->deleteLater();
            chartGroup->setMesh(nullptr);
        }
        return;
    }

    if(!chartMesh) {
        chartMesh = new A3D::Mesh();
        chartMesh->setParent(chartGroup);
        chartGroup->setMesh(chartMesh);
    }

    std::vector<float> const& xAxisPos     = m_mapChart.normalizedValuesForAxis(AXIS_X);
    std::vector<float> const& zAxisPos     = m_mapChart.normalizedValuesForAxis(AXIS_Z);
    std::vector<float> const& yValueHeight = m_mapChart.normalizedValuesForAxis(AXIS_Y);

    ChartAxisType const xAxisType = m_mapChart.axisData(AXIS_X).type();
    ChartAxisType const zAxisType = m_mapChart.axisData(AXIS_Z).type();

    size_t const xAxisSize = xAxisPos.size();
    size_t const zAxisSize = zAxisPos.size();

    // x*z = 0 should return an empty mesh right here...

    float const calcXEnumStripThickness = (1.f / xAxisSize) * m_enumStripThicknessX;
    float const calcZEnumStripThickness = (1.f / zAxisSize) * m_enumStripThicknessZ;

    chartMesh->setDrawMode(A3D::Mesh::Triangles);

    std::vector<Mesh::Vertex>& vertices = chartMesh->vertices();
    vertices.clear();

    size_t xIterations = xAxisSize - 1;
    if(xAxisType == CHAXIS_ENUMERATED)
        ++xIterations;

    size_t zIterations = zAxisSize - 1;
    if(zAxisType == CHAXIS_ENUMERATED)
        ++zIterations;

    enum TriangulationType {
        TRI_5_VERTICES,
        TRI_4_VERTICES,
    };

    TriangulationType triangulationType = TRI_5_VERTICES;
    if(xAxisType == CHAXIS_ENUMERATED && zAxisType == CHAXIS_ENUMERATED)
        triangulationType = TRI_4_VERTICES;

    switch(triangulationType) {
    case TRI_4_VERTICES:
        vertices.reserve(zIterations * xIterations * 6);
        break;
    case TRI_5_VERTICES:
        vertices.reserve(zIterations * xIterations * 12);
        break;
    }

    for(size_t z = 0; z < zIterations; ++z) {
        for(size_t x = 0; x < xIterations; ++x) {
            A3D::Mesh::Vertex xLow_zLow;   // Top Left
            A3D::Mesh::Vertex xHigh_zLow;  // Top Right
            A3D::Mesh::Vertex xLow_zHigh;  // Bottom Left
            A3D::Mesh::Vertex xHigh_zHigh; // Bottom Right

            size_t xLow_index  = x;
            size_t xHigh_index = x + 1;
            size_t zLow_index  = z;
            size_t zHigh_index = z + 1;

            if(xAxisType == CHAXIS_ENUMERATED)
                --xHigh_index;
            if(zAxisType == CHAXIS_ENUMERATED)
                --zHigh_index;

            getVertexFromCoordinate(xLow_zLow, xAxisPos, zAxisPos, yValueHeight, xLow_index, zLow_index);
            getVertexFromCoordinate(xHigh_zLow, xAxisPos, zAxisPos, yValueHeight, xHigh_index, zLow_index);
            getVertexFromCoordinate(xLow_zHigh, xAxisPos, zAxisPos, yValueHeight, xLow_index, zHigh_index);
            getVertexFromCoordinate(xHigh_zHigh, xAxisPos, zAxisPos, yValueHeight, xHigh_index, zHigh_index);

            if(xAxisType == CHAXIS_ENUMERATED) {
                float const beforeThickness = xLow_zLow.Position3D.x() * calcXEnumStripThickness;
                float const afterThickness  = calcXEnumStripThickness - beforeThickness;

                xLow_zLow.Position3D.setX(xLow_zLow.Position3D.x() - beforeThickness);
                xLow_zHigh.Position3D.setX(xLow_zHigh.Position3D.x() - beforeThickness);
                xHigh_zLow.Position3D.setX(xHigh_zLow.Position3D.x() + afterThickness);
                xHigh_zHigh.Position3D.setX(xHigh_zHigh.Position3D.x() + afterThickness);
            }

            if(zAxisType == CHAXIS_ENUMERATED) {
                float const beforeThickness = xLow_zLow.Position3D.z() * calcZEnumStripThickness;
                float const afterThickness  = calcZEnumStripThickness - beforeThickness;

                xLow_zLow.Position3D.setZ(xLow_zLow.Position3D.z() - beforeThickness);
                xHigh_zLow.Position3D.setZ(xHigh_zLow.Position3D.z() - beforeThickness);
                xLow_zHigh.Position3D.setZ(xLow_zHigh.Position3D.z() + afterThickness);
                xHigh_zHigh.Position3D.setZ(xHigh_zHigh.Position3D.z() + afterThickness);
            }

            if(triangulationType == TRI_4_VERTICES) {
                // X or Z is enumerated: No need to generate the central vertex, the 2-triangle approximation is more than enough.
                generateNormal(xHigh_zLow, xLow_zLow, xLow_zHigh);
                vertices.push_back(xHigh_zLow);
                vertices.push_back(xLow_zLow);
                vertices.push_back(xLow_zHigh);

                generateNormal(xLow_zHigh, xHigh_zHigh, xHigh_zLow);
                vertices.push_back(xLow_zHigh);
                vertices.push_back(xHigh_zHigh);
                vertices.push_back(xHigh_zLow);
            }
            else {
                // Both X and Z are interpolated: We need the center vertex too.
                A3D::Mesh::Vertex center;
                center.Position2D     = (xLow_zLow.Position2D + xHigh_zLow.Position2D + xLow_zHigh.Position2D + xHigh_zHigh.Position2D) * 0.25f;
                center.Position3D     = (xLow_zLow.Position3D + xHigh_zLow.Position3D + xLow_zHigh.Position3D + xHigh_zHigh.Position3D) * 0.25f;
                center.TextureCoord2D = (xLow_zLow.TextureCoord2D + xHigh_zLow.TextureCoord2D + xLow_zHigh.TextureCoord2D + xHigh_zHigh.TextureCoord2D) * 0.25f;
                center.Color3D        = (xLow_zLow.Color3D + xHigh_zLow.Color3D + xLow_zHigh.Color3D + xHigh_zHigh.Color3D) * 0.25f;
                center.Color4D        = (xLow_zLow.Color4D + xHigh_zLow.Color4D + xLow_zHigh.Color4D + xHigh_zHigh.Color4D) * 0.25f;

                generateNormal(xHigh_zLow, xLow_zLow, center);
                vertices.push_back(xHigh_zLow);
                vertices.push_back(xLow_zLow);
                vertices.push_back(center);

                generateNormal(xLow_zLow, xLow_zHigh, center);
                vertices.push_back(xLow_zLow);
                vertices.push_back(xLow_zHigh);
                vertices.push_back(center);

                generateNormal(xLow_zHigh, xHigh_zHigh, center);
                vertices.push_back(xLow_zHigh);
                vertices.push_back(xHigh_zHigh);
                vertices.push_back(center);

                generateNormal(xHigh_zHigh, xHigh_zLow, center);
                vertices.push_back(xHigh_zHigh);
                vertices.push_back(xHigh_zLow);
                vertices.push_back(center);
            }
        }
    }

    chartMesh->setContents(A3D::Mesh::Position3D | A3D::Mesh::Normal3D | A3D::Mesh::TextureCoord2D);
    chartMesh->setRenderOptions(A3D::Mesh::DisableCulling);
    chartMesh->invalidateCache();
}

void SurfaceChartEntity::updateIndicatorLines(Axis3D axis) {
    if(axis >= AXIS_COUNT)
        return;

    QPointer<LineGroup>(&indicatorLines)[CHAXIND_COUNT] = m_indicatorLines[axis];
    Axis3D otherAxisA, otherAxisB;

    switch(axis) {
    case AXIS_X:
        otherAxisA = AXIS_Y;
        otherAxisB = AXIS_Z;
        break;
    case AXIS_Y:
        otherAxisA = AXIS_X;
        otherAxisB = AXIS_Z;
        break;
    case AXIS_Z:
        otherAxisA = AXIS_X;
        otherAxisB = AXIS_Y;
        break;
    default:
        return;
    }

    for(QPointer<LineGroup>& lineGroup: indicatorLines) {
        lineGroup->vertices().clear();
    }

    for(ChartAxisIndicator const& indicator: m_mapChart.axisData(axis).indicators()) {
        if(indicator.m_type >= CHAXIND_COUNT)
            continue;

        LineGroup::Vertex vertexBase;
        vertexBase.Color4D = QVector4D(1.f, 0.5f, 0.f, 1.f);
        setVectorAxis(vertexBase.Position3D, axis, indicator.m_normalizedValue);

        LineGroup::Vertex vxAxisA = vertexBase;
        LineGroup::Vertex vxAxisB = vertexBase;
        setVectorAxis(vxAxisA.Position3D, otherAxisA, 1.f);
        setVectorAxis(vxAxisB.Position3D, otherAxisB, 1.f);

        std::vector<A3D::LineGroup::Vertex>& vertices = indicatorLines[indicator.m_type]->vertices();
        vertices.push_back(vertexBase);
        vertices.push_back(vxAxisA);
        vertices.push_back(vertexBase);
        vertices.push_back(vxAxisB);
    }

    for(QPointer<LineGroup>& lineGroup: indicatorLines) {
        lineGroup->setContents(LineGroup::Position3D | LineGroup::Color4D);
        lineGroup->invalidateCache();
    }
}

void SurfaceChartEntity::updateIndicatorLabels(Axis3D axis) {
    if(axis >= AXIS_COUNT)
        return;

    std::vector<QPointer<TextBillboardEntity>>& indicatorLabels = m_indicatorLabels[axis];

    Axis3D otherAxisA, otherAxisB;

    switch(axis) {
    case AXIS_X:
        otherAxisA = AXIS_Y;
        otherAxisB = AXIS_Z;
        break;
    case AXIS_Y:
        otherAxisA = AXIS_X;
        otherAxisB = AXIS_Z;
        break;
    case AXIS_Z:
        otherAxisA = AXIS_X;
        otherAxisB = AXIS_Y;
        break;
    default:
        return;
    }

    float const otherAxisDistanceA = getVectorAxis(m_labelDistances, otherAxisA);
    float const otherAxisDistanceB = getVectorAxis(m_labelDistances, otherAxisB);

    for(QPointer<TextBillboardEntity>& previousLabel: indicatorLabels) {
        if(previousLabel) {
            delete previousLabel;
        }
    }
    indicatorLabels.clear();

    for(ChartAxisIndicator const& indicator: m_mapChart.axisData(axis).indicators()) {
        if(indicator.m_label.isEmpty())
            continue;

        QVector3D vertexBase;
        setVectorAxis(vertexBase, axis, indicator.m_normalizedValue);

        QVector3D vxAxisA = vertexBase;
        QVector3D vxAxisB = vertexBase;
        setVectorAxis(vxAxisA, otherAxisA, 1.f + otherAxisDistanceA);
        setVectorAxis(vxAxisB, otherAxisB, 1.f + otherAxisDistanceB);

        TextBillboardEntity* newBillboardA = emplaceChildEntity<TextBillboardEntity>();
        TextBillboardEntity* newBillboardB = emplaceChildEntity<TextBillboardEntity>();

        newBillboardA->setText(indicator.m_label);
        newBillboardA->setColor(Qt::white);
        newBillboardA->setFont(QFont("Arial", 64));
        newBillboardA->setScale(QVector3D(0.05f, 0.05f, 0.05f));
        newBillboardA->setPosition(vxAxisA);

        newBillboardB->setText(indicator.m_label);
        newBillboardB->setColor(Qt::white);
        newBillboardB->setFont(QFont("Arial", 64));
        newBillboardB->setScale(QVector3D(0.05f, 0.05f, 0.05f));
        newBillboardB->setPosition(vxAxisB);

        indicatorLabels.push_back(newBillboardA);
        indicatorLabels.push_back(newBillboardB);
    }
}

void SurfaceChartEntity::setMarker(QVector2D marker) {
    m_marker              = marker;
    QVector3D startVector = QVector3D(marker.x(), 0.f, marker.y());
    QVector3D endVector   = QVector3D(marker.x(), 1.f, marker.y());

    m_markerLineGroup->vertices().clear();
    m_markerLineGroup->indices().clear();

	LineGroup::Vertex start;
	LineGroup::Vertex end;
	start.Position3D = startVector;
	start.Color4D    = QVector4D(0.5, 0, 1, 1);
	end.Position3D   = endVector;
	end.Color4D      = QVector4D(0.5, 0, 1, 1);

    m_markerLineGroup->vertices().push_back(start);
    m_markerLineGroup->vertices().push_back(end);
    m_markerLineGroup->invalidateCache();
}

QVector2D SurfaceChartEntity::marker() const {
    return m_marker;
}

}
