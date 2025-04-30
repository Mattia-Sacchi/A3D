#include "maps.h"

namespace A3D {

bool Map::setLinearAxis(Direction3D dir, float min, float max, size_t ticks, size_t chartTicks) {
	if(dir >= A3D::D_Positive_Count) {
		return false;
	}

	if(chartTicks)
		m_axes[dir] = AxisData(getLinearAxis(min, max, ticks - 1), min, max, chartTicks);
	else
		m_axes[dir] = AxisData(getLinearAxis(min, max, ticks - 1), min, max);
	return true;
}

bool Map::setFixedAxis(Direction3D dir, QStringList list) {
	if(dir >= A3D::D_Positive_Count) {
		return false;
	}
	m_axes[dir] = list;
	return true;
}

bool Map::setAxis(Direction3D dir, std::vector<float> data, size_t chartTicks) {
	if(dir >= D_Positive_Count) {
		return false;
	}
	if(chartTicks)
		m_axes[dir] = AxisData(data, chartTicks);
	else
		m_axes[dir] = data;
	return true;
}

bool Map::setData(std::vector<float> data) {
	m_data = data;

	if(m_data.size() != m_axes[D_X_Axis].size() * m_axes[D_Z_Axis].size()                // Check on final dimension
	   && m_axes[D_X_Axis].size() && m_axes[D_Z_Axis].size() && m_axes[D_Y_Axis].size()) // Check of presence
		return false;
	return m_isValid = true;
}

}
