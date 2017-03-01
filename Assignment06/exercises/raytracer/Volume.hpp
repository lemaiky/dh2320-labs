#pragma once
#include "raytracerConfig.hpp"
#include "Math.hpp"
#include <vector>
#include <map>

namespace rt{

	class Volume
	{
	public:
		RAYTRACER_EXPORTS Volume(void);
		RAYTRACER_EXPORTS virtual ~Volume(void);
		RAYTRACER_EXPORTS void initData(const size_t size_x, const size_t size_y, const size_t size_z);
		RAYTRACER_EXPORTS void setDataAt(const size_t x, const size_t y, const size_t z, const double val);
		RAYTRACER_EXPORTS double F(const size_t x, const size_t y, const size_t z) const;
		RAYTRACER_EXPORTS Vec3i getVolumeDims() const;
		RAYTRACER_EXPORTS size_t getNumVoxels() const;
		RAYTRACER_EXPORTS void normalize();
		
	private:
		size_t L, M, N;
		std::vector<double> m_data;
		double MaxVal;
		double MinVal;
	};

}
