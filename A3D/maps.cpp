#include "maps.h"

namespace A3D {

bool Map::setLinearAxis(Direction3D dir, float min, float max, size_t ticks) {
	if(dir >= A3D::D_Positive_Count) {
		return false;
	}
	m_axes[dir] = getLinearAxis(min, max, ticks-1);
	return true;
}

bool Map::setFixedAxis(Direction3D dir, QStringList list) {
	if(dir >= A3D::D_Positive_Count) {
		return false;
	}
	m_axes[dir] = list;
	return true;
}

bool Map::setAxis(Direction3D dir, std::vector<float> data) {
	if(dir >= D_Positive_Count) {
		return false;
	}
	m_axes[dir] = data;
	return true;
}

std::vector<float> Map::getLinearAxis(float min, float max, size_t ticks) const {
	std::vector<float> data;
	data.clear();
	data.reserve(ticks);

	float const multiplier       = 1 / static_cast<float>(ticks);
	float const strokeMultiplier = (max - min) / ticks;

	for(size_t i = 0; i <= ticks; i++)
		data.push_back((strokeMultiplier * i) + min);

	return data;
}

bool Map::setData(std::vector<float> data) {
	m_data = data;

	if(m_data.size() != m_axes[D_X_Axis].size() * m_axes[D_Z_Axis].size()                // Check on final dimension
	   && m_axes[D_X_Axis].size() && m_axes[D_Z_Axis].size() && m_axes[D_Y_Axis].size()) // Check of presence
		return false;
	return m_isValid = true;
}

}
