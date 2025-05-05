#include "chart.h"

namespace A3D {

ChartAxisIndicatorStyle::ChartAxisIndicatorStyle(QColor indicatorColor, QColor labelColor, QFont labelFont, float labelSize)
	: m_indicatorColor(std::move(indicatorColor)),
	  m_labelColor(std::move(labelColor)),
	  m_labelFont(std::move(labelFont)),
	  m_labelSize(labelSize) {}

ChartAxisIndicator::ChartAxisIndicator(ChartAxisIndicatorType type, float value, float normalizedValue, QString label, ChartAxisIndicatorStyle style)
	: m_type(type),
	  m_value(value),
	  m_normalizedValue(normalizedValue),
	  m_label(std::move(label)),
	  m_style(std::move(style)) {}

ChartAxisStyledEnumeratedIndicatorData::ChartAxisStyledEnumeratedIndicatorData(QString label, ChartAxisIndicatorStyle style)
	: m_label(std::move(label)),
	  m_style(std::move(style)) {}

ChartAxisData::ChartAxisData()
	: m_type(CHAXIS_LINEAR_INTERPOLATED),
	  m_axisMinimumValue(0.f),
	  m_axisMaximumValue(1.f) {}

void ChartAxisData::setName(QString name) {
	m_name = std::move(name);
}

QString ChartAxisData::name() const {
	return m_name;
}

void ChartAxisData::setDefaultIndicatorStyle(ChartAxisIndicatorStyle style) {
	m_defaultStyle = std::move(style);
}

ChartAxisIndicatorStyle ChartAxisData::defaultIndicatorStyle() const {
	return m_defaultStyle;
}

void ChartAxisData::removeAllIndicators() {
	m_indicators.clear();
}

void ChartAxisData::removeIndicators(ChartAxisIndicatorType indicatorType) {
	m_indicators.erase(
		std::remove_if(
			m_indicators.begin(), m_indicators.end(),
			[indicatorType](ChartAxisIndicator const& indicator) -> bool {
				return indicator.m_type == indicatorType;
			}
		),
		m_indicators.end()
	);
}

void ChartAxisData::setIndicators(std::vector<ChartAxisIndicator> points) {
	m_type = CHAXIS_LINEAR_INTERPOLATED;
	m_indicators.clear();
	m_indicators.reserve(points.size());

	for(ChartAxisIndicator& indicator: points)
		m_indicators.emplace_back(std::move(indicator));
}

void ChartAxisData::setIndicators(QStringList enumeratedPoints, ChartAxisIndicatorType indicatorType) {
	if(enumeratedPoints.isEmpty())
		return;

	m_type = CHAXIS_ENUMERATED;
	m_indicators.clear();
	m_indicators.reserve(enumeratedPoints.size());

	m_axisMinimumValue = 0.f;
	m_axisMaximumValue = static_cast<float>(enumeratedPoints.size() - 1);
	for(std::size_t i = 0; i < enumeratedPoints.size(); ++i) {
		m_indicators.push_back(ChartAxisIndicator(indicatorType, static_cast<float>(i), static_cast<float>(i) / m_axisMaximumValue, std::move(enumeratedPoints[i]), m_defaultStyle)
		);
	}
}

void ChartAxisData::setIndicators(std::vector<ChartAxisStyledEnumeratedIndicatorData> styledEnumeratedPoints, ChartAxisIndicatorType indicatorType) {
	if(styledEnumeratedPoints.empty())
		return;

	m_type = CHAXIS_ENUMERATED;
	m_indicators.clear();
	m_indicators.reserve(styledEnumeratedPoints.size());

	m_axisMinimumValue = 0.f;
	m_axisMaximumValue = static_cast<float>(styledEnumeratedPoints.size() - 1);
	for(std::size_t i = 0; i < styledEnumeratedPoints.size(); ++i) {
		m_indicators.push_back(ChartAxisIndicator(
			indicatorType, static_cast<float>(i), static_cast<float>(i) / m_axisMaximumValue, std::move(styledEnumeratedPoints[i].m_label),
			std::move(styledEnumeratedPoints[i].m_style)
		));
	}
}

void ChartAxisData::addIndicators(std::vector<float> const& points, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;
	m_indicators.reserve(m_indicators.size() + points.size());

	for(float const& val: points)
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
}

void ChartAxisData::addEquidistantIndicatorsByIndicatorCount(float from, float to, size_t indicatorCount, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED || indicatorCount < 2)
		return;

	m_indicators.reserve(m_indicators.size() + indicatorCount);

	float fInverseIndicatorCount = 1.f / static_cast<float>(indicatorCount - 1);
	for(size_t i = 0; i < indicatorCount; ++i) {
		float const val = from + ((to - from) * fInverseIndicatorCount * static_cast<float>(i));
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
	}
}

void ChartAxisData::addEquidistantIndicatorsByStepSize(float from, float to, float stepSize, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;

	size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize);

	if(indicatorCount > 1000) // Too many, limit to 1000 points...
	{
		addEquidistantIndicatorsByIndicatorCount(from, to, 1000, toStringPrecision, indicatorType);
		return;
	}

	m_indicators.reserve(m_indicators.size() + indicatorCount + 1);

	for(size_t i = 0; i < indicatorCount; ++i) {
		float const val = from + stepSize * static_cast<float>(i);
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
	}

	float const val = to;
	m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
}

QString ChartAxisData::getEnumerationName(std::size_t index) const {
	if(m_type != CHAXIS_ENUMERATED)
		return 0;

	if(index >= m_indicators.size())
		return 0;

	return m_indicators[index].m_label;
}

std::size_t ChartAxisData::getEnumerationsCount() const {
	if(m_type != CHAXIS_ENUMERATED)
		return 0;

	return m_indicators.size();
}

float ChartAxisData::denormalizeValue(float normalizedValue) const {
	if(m_type == CHAXIS_LINEAR_INTERPOLATED) {
		return (normalizedValue * (m_axisMaximumValue - m_axisMinimumValue)) + m_axisMinimumValue;
	}
	else if(m_type == CHAXIS_ENUMERATED) {
		float range = (m_axisMaximumValue - m_axisMinimumValue);

		if(range >= 0) {
			range += 1.f;
			float val = (normalizedValue * range) + m_axisMinimumValue;
			return std::floor(val);
		}
		else {
			range -= 1.f;
			float val = (normalizedValue * range) + m_axisMinimumValue;
			return std::ceil(val);
		}
	}
	return 0.f;
}

void ChartAxisData::setMinMax(float minimum, float maximum) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;

	m_axisMinimumValue = minimum;
	m_axisMaximumValue = maximum;

	normalizeIndicatorValues();
}

void ChartAxisData::invert() {
	std::swap(m_axisMaximumValue, m_axisMinimumValue);

	for(ChartAxisIndicator& indicator: m_indicators) {
		indicator.m_normalizedValue = 1.f - indicator.m_normalizedValue;
	}
}

ChartAxisType ChartAxisData::type() const {
	return m_type;
}

std::vector<ChartAxisIndicator> const& ChartAxisData::indicators() const {
	return m_indicators;
}

float ChartAxisData::minimum() const {
	return m_axisMinimumValue;
}
float ChartAxisData::maximum() const {
	return m_axisMaximumValue;
}

float ChartAxisData::normalizedIndicatorValue(float input) const {
	return (input - minimum()) / (maximum() - minimum());
}

void ChartAxisData::normalizeIndicatorValues() {
	float const fZeroOffset = minimum();
	float const fInvDelta   = 1.f / (maximum() - fZeroOffset);
	for(ChartAxisIndicator& val: m_indicators) {
		val.m_normalizedValue = ((val.m_value - fZeroOffset) * fInvDelta) + fZeroOffset;
	}
}

MapChart3D::MapChart3D()
	: m_isValid(false) {}

bool MapChart3D::isValid() const {
	return m_isValid;
}

void MapChart3D::setAxisData(Axis3D axis, ChartAxisData data) {
	if(axis >= AXIS_COUNT)
		return;
	m_axes[axis] = std::move(data);
	normalizeAxisPoints(axis);
}

ChartAxisData const& MapChart3D::axisData(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_axes[axis];

	static ChartAxisData invalid;
	return invalid;
}

void MapChart3D::setChartPoints(std::vector<float> x_input_positions, std::vector<float> z_input_positions, std::vector<float> y_values) {
	if(y_values.size() != (x_input_positions.size() * z_input_positions.size()))
		return;

	m_values[AXIS_X] = std::move(x_input_positions);
	m_values[AXIS_Y] = std::move(y_values);
	m_values[AXIS_Z] = std::move(z_input_positions);

	normalizeAxisPoints(AXIS_X);
	normalizeAxisPoints(AXIS_Y);
	normalizeAxisPoints(AXIS_Z);

	m_isValid = true;
}

std::vector<float> const& MapChart3D::valuesForAxis(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_values[axis];

	static std::vector<float> invalid;
	return invalid;
}
std::vector<float> const& MapChart3D::normalizedValuesForAxis(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_normalized_values[axis];

	static std::vector<float> invalid;
	return invalid;
}

QVector3D MapChart3D::getValueFromMesh(QVector2D const& meshCoordinate) const {
	if(!isValid())
		return QVector3D();

	QVector2D localCoordinate = QVector2D(m_axes[AXIS_X].denormalizeValue(meshCoordinate.x()), m_axes[AXIS_Z].denormalizeValue(meshCoordinate.y()));

	return getValueFromInput(localCoordinate);
}

QVector3D MapChart3D::getValueFromInput(QVector2D const& localCoordinate) const {
	if(!isValid())
		return QVector3D();

	int leftX_index    = -1;
	int rightX_index   = -1;
	float leftX_weight = 0.f;

	for(std::size_t iX = 0; iX < m_values[AXIS_X].size(); ++iX) {
		if(m_values[AXIS_X][iX] == localCoordinate.x()) {
			leftX_index  = static_cast<int>(iX);
			leftX_weight = 1.f;
			break;
		}
		else if(m_values[AXIS_X][iX] > localCoordinate.x()) {
			if(iX == 0) {
				leftX_index  = static_cast<int>(iX);
				leftX_weight = 1.f;
				break;
			}
			else {
				leftX_index  = static_cast<int>(iX - 1);
				rightX_index = static_cast<int>(iX);

				float range      = m_values[AXIS_X][iX] - m_values[AXIS_X][iX - 1];
				float posInRange = localCoordinate.x() - m_values[AXIS_X][iX - 1];

				leftX_weight = 1.f - (static_cast<float>(posInRange) / static_cast<float>(range));
				break;
			}
		}
	}

	if(leftX_index < 0) {
		leftX_index  = static_cast<int>(m_values[AXIS_X].size() - 1);
		leftX_weight = 1.f;
	}

	int leftZ_index    = -1;
	int rightZ_index   = -1;
	float leftZ_weight = 0.f;

	for(std::size_t iZ = 1; iZ < m_values[AXIS_Z].size(); ++iZ) {
		if(m_values[AXIS_Z][iZ] == localCoordinate.y()) {
			leftZ_index  = static_cast<int>(iZ);
			leftZ_weight = 1.f;
			break;
		}
		else if(m_values[AXIS_Z][iZ] > localCoordinate.y()) {
			if(iZ == 0) {
				leftZ_index  = static_cast<int>(iZ);
				leftZ_weight = 1.f;
				break;
			}
			else {
				leftZ_index  = static_cast<int>(iZ - 1);
				rightZ_index = static_cast<int>(iZ);

				float range      = m_values[AXIS_Z][iZ] - m_values[AXIS_Z][iZ - 1];
				float posInRange = localCoordinate.y() - m_values[AXIS_Z][iZ - 1];

				leftZ_weight = 1.f - (static_cast<float>(posInRange) / static_cast<float>(range));
				break;
			}
		}
	}

	if(leftZ_index < 0) {
		leftZ_index  = static_cast<int>(m_values[AXIS_Z].size() - 1);
		leftZ_weight = 1.f;
	}

	// Calculate matrix

	float const rightX_weight = 1.f - leftX_weight;
	float const rightZ_weight = 1.f - leftZ_weight;

	float fMatrixResult = m_values[AXIS_Y][leftX_index + (leftZ_index * m_values[AXIS_X].size())] * leftX_weight * leftZ_weight;

	if(rightX_index >= 0)
		fMatrixResult += m_values[AXIS_Y][rightX_index + (leftZ_index * m_values[AXIS_X].size())] * rightX_weight * leftZ_weight;

	if(rightZ_index >= 0)
		fMatrixResult += m_values[AXIS_Y][leftX_index + (rightZ_index * m_values[AXIS_X].size())] * leftX_weight * rightZ_weight;

	if(rightX_index >= 0 && rightZ_index >= 0)
		fMatrixResult += m_values[AXIS_Y][rightX_index + (rightZ_index * m_values[AXIS_X].size())] * rightX_weight * rightZ_weight;

	return QVector3D(localCoordinate.x(), fMatrixResult, localCoordinate.y());
}

void MapChart3D::normalizeAxisPoints(Axis3D axis) {
	if(axis >= AXIS_COUNT)
		return;

	m_normalized_values[axis] = m_values[axis];

	float const fZeroOffset = m_axes[axis].minimum();
	float const fInvDelta   = 1.f / (m_axes[axis].maximum() - m_axes[axis].minimum());
	for(float& val: m_normalized_values[axis]) {
		val = (val - fZeroOffset) * fInvDelta;
	}
}

}
