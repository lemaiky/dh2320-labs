#include "Volume.hpp"
#include <cassert>
#include <algorithm>

namespace rt{

	Volume::Volume(void)
	{
		MaxVal = DBL_MIN;
		MinVal = DBL_MAX;
	}

	Volume::~Volume(void)
	{
	}

	void Volume::initData(const size_t size_x, const size_t size_y, const size_t size_z){
		L = size_x;
		M = size_y;
		N = size_z;
		m_data.clear();
		m_data.resize(L * M * N, 0);
	}

	void Volume::setDataAt(const size_t _x, const size_t _y, const size_t _z, const double val){
		m_data[L * (M * _z + _y) + _x] = val;
		MaxVal = std::max(MaxVal, val);
		MinVal = std::min(MinVal, val);
	}

	double Volume::F(const size_t x, const size_t y, const size_t z) const {
		return m_data[L * (M * z + y) + x];
	}

	Vec3i Volume::getVolumeDims() const{
		return Vec3i(L, M, N);
	}

	size_t Volume::getNumVoxels() const{
		return L * M * N;
	}

	void Volume::normalize(){
		for (size_t i = 0; i < m_data.size(); i++){
			m_data[i] -= MinVal;
			m_data[i] /= (MaxVal - MinVal);
		}
	}

}