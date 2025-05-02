#ifndef SURFACECHARTENTITY_H
#define SURFACECHARTENTITY_H

#include "linegroup.h"
#include "entity.h"
#include "chart.h"
#include "textbillboardentity.h"
#include <QObject>

namespace A3D {

class SurfaceChartEntity : public Entity {
	Q_OBJECT
public:
    SurfaceChartEntity(Entity* parent = nullptr);

    void setChart(MapChart3D map);
    MapChart3D const& mapChart() const;

    // Range: 0-1 -> 0-100%
    void setEnumStripThickness(float xThickness, float zThickness);
    float enumStripThicknessX() const;
    float enumStripThicknessZ() const;

    // X component means distance of labels on the X axis, and so on.
    void setLabelDistances(QVector3D distance_x_y_z);
    QVector3D labelDistances() const;

    

    void setMarker(QVector2D marker);
    QVector2D marker() const;

    void setIndicatorsColor(QVector4D color);
    QVector4D indicatorsColor() const;

    void setLabelColor(QColor color);
    QColor labelColor() const;

    void setLabelFont(QFont font);
    QFont labelFont() const;

private:
    void updateSurfaceMesh();
    void updateAxesLines();
    void updateIndicatorLines(Axis3D axis);
    void updateAxesLabels();
    void updateIndicatorLabels(Axis3D axis);

    void updateIndicatorLinesColor(Axis3D axis);
    void updateIndicatorLabelsColor(Axis3D axis);
    void updateIndicatorLabelsFont(Axis3D axis);

    MapChart3D m_mapChart;
    float m_enumStripThicknessX;
    float m_enumStripThicknessZ;

    QVector3D m_labelDistances;

    QPointer<LineGroup> m_indicatorLines[AXIS_COUNT][CHAXIND_COUNT];
    std::vector<QPointer<TextBillboardEntity>> m_indicatorLabels[AXIS_COUNT];

    QVector2D m_marker;
    LineGroup* m_markerLineGroup;

    QVector4D m_indicatorColor;
    QColor m_labelColor;
    QFont m_labelFont;

};

}

#endif // SURFACECHARTENTITY_H
