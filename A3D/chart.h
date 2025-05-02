#ifndef CHART_H
#define CHART_H

#include "common.h"

namespace A3D {

enum ChartAxisType {
    CHAXIS_LINEAR_INTERPOLATED,
    CHAXIS_ENUMERATED,
};

enum ChartAxisIndicatorType {
    CHAXIND_MAJOR_INDICATOR,
    CHAXIND_MINOR_INDICATOR,
    CHAXIND_COUNT,
};

struct ChartAxisIndicator {
    ChartAxisIndicator(ChartAxisIndicatorType type = CHAXIND_MAJOR_INDICATOR, float value = 0.f, float normalizedValue = 0.f, QString label = QString());
    ChartAxisIndicator(ChartAxisIndicator const&)            = default;
    ChartAxisIndicator(ChartAxisIndicator&&)                 = default;
    ChartAxisIndicator& operator=(ChartAxisIndicator const&) = default;
    ChartAxisIndicator& operator=(ChartAxisIndicator&&)      = default;

    ChartAxisIndicatorType m_type;
    float m_value;
    float m_normalizedValue;
    QString m_label;
};

class ChartAxisData {
public:
    ChartAxisData();
    ChartAxisData(ChartAxisData const&)            = default;
    ChartAxisData(ChartAxisData&&)                 = default;
    ChartAxisData& operator=(ChartAxisData const&) = default;
    ChartAxisData& operator=(ChartAxisData&&)      = default;

    void setName(QString name);
    QString name() const;

    void removeAllIndicators();
    void removeIndicators(ChartAxisIndicatorType indicatorType);

    // Sets the Type to CHAXIS_LINEAR_INTERPOLATED and adds all points.
    void setIndicators(std::vector<ChartAxisIndicator> points);

    // Sets the Type to CHAXIS_ENUMERATED and adds all points.
    // Also updates min/max values.
    // Only works when enumeratedPoints is not empty.
    void setIndicators(QStringList enumeratedPoints, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

    // Only works when Type is CHAXIS_LINEAR_INTERPOLATED.
    void addIndicators(std::vector<float> const& points, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

    // Only works when Type is CHAXIS_LINEAR_INTERPOLATED.
    void addEquidistantIndicatorsByIndicatorCount(
        float from, float to, size_t indicatorCount, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR
    );

    // Only works when Type is CHAXIS_LINEAR_INTERPOLATED.
    void addEquidistantIndicatorsByStepSize(float from, float to, float stepSize, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

    // Only works when Type is CHAXIS_LINEAR_INTERPOLATED.
    void setMinMax(float minimum, float maximum);

    // Only works when Type is CHAXIS_ENUMERATED.
    QString getEnumerationName(std::size_t index) const;

    // Only works when Type is CHAXIS_ENUMERATED.
    std::size_t getEnumerationsCount() const;

    // Returns interpolated numbers for CHAXIS_LINEAR_INTERPOLATED, enumerated values for CHAXIS_ENUMERATED.
    float denormalizeValue(float normalizedValue) const;

    // Inverts the axis
    void invert();

    ChartAxisType type() const;
    std::vector<ChartAxisIndicator> const& indicators() const;
    float minimum() const;
    float maximum() const;

private:
    float normalizedIndicatorValue(float input) const;
    void normalizeIndicatorValues();

    QString m_name;
    ChartAxisType m_type;
    std::vector<ChartAxisIndicator> m_indicators;

    float m_axisMinimumValue;
    float m_axisMaximumValue;
};

struct Chart3DSearchResult {
    float m_interpolatedValue;
    float m_closestValue;
    std::size_t m_closestIndex;
};

struct Chart3DRadiusSearchResult {
    Chart3DSearchResult m_searchResult;

    struct Point {
        std::size_t m_index;
        float m_weight;
    };

    // Includes the closestValue
    std::vector<Point> m_points;
};

class MapChart3D {
public:
    MapChart3D();
    MapChart3D(MapChart3D const&)            = default;
    MapChart3D(MapChart3D&&)                 = default;
    MapChart3D& operator=(MapChart3D const&) = default;
    MapChart3D& operator=(MapChart3D&&)      = default;

    bool isValid() const;

    void setAxisData(Axis3D axis, ChartAxisData data);
    ChartAxisData const& axisData(Axis3D axis) const;

    // y_values should contain height value, the vector should be x*z size.
    // y_values rows are x-adjacent values, where y_values columns are z-adjacent values.
    void setChartPoints(std::vector<float> x_input_positions, std::vector<float> z_input_positions, std::vector<float> y_values);
    std::vector<float> const& valuesForAxis(Axis3D axis) const;
    std::vector<float> const& normalizedValuesForAxis(Axis3D axis) const;

    QVector3D getValueFromMesh(QVector2D const& meshCoordinate) const;
    QVector3D getValueFromInput(QVector2D const& inputCoordinate) const;
    Chart3DSearchResult getNearestIndex(QVector3D const& localCoordinate) const;
    Chart3DRadiusSearchResult getNearestIndicesWithRadius(QVector3D const& localCoordinate, float radius) const;

private:
    void normalizeAxisPoints(Axis3D axis);

    bool m_isValid;
    ChartAxisData m_axes[AXIS_COUNT];
    std::vector<float> m_values[AXIS_COUNT];
    std::vector<float> m_normalized_values[AXIS_COUNT];
};

}

#endif // CHART_H
