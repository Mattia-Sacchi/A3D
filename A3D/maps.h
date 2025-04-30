#ifndef A3DMAPS_H
#define A3DMAPS_H

#include <vector>
#include "common.h"

namespace A3D {
class Map {
public:
	Map()
		: m_isValid(false) {}

	bool setLinearAxis(Direction3D dir, float min, float max, size_t ticks);
	bool setFixedAxis(Direction3D dir, QStringList list);
	bool setAxis(Direction3D dir, std::vector<float> data);
	// Last function to call check if everything is ok
	bool setData(std::vector<float> data);
	inline std::vector<float> data() const { return m_data; }

	inline AxisData getAxis(Direction3D dir)  const{
		if(dir >= D_Positive_Count)
			return AxisData();
		return m_axes[dir];
	}

	inline bool isValid() const { return m_isValid; }

private:
	std::array<AxisData, A3D::D_Positive_Count> m_axes;
	std::vector<float> m_data;
	bool m_isValid;
	std::vector<float> getLinearAxis(float min, float max, size_t ticks = 10) const;
};
}
#endif
