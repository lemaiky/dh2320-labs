#pragma once
#include "raytracerConfig.hpp"
#include "Math.hpp"
#include <vector>
#include <map>

namespace rt{

	typedef struct TFKey {
		double intensity; // must lie between 0 and 1
		Vec4d color;
		RAYTRACER_EXPORTS TFKey(double intensity_, Vec4d color_){
			intensity = intensity_;
			color = color_;
		}
	} TFKey;

	class TransferFunction
	{
	public:
		RAYTRACER_EXPORTS TransferFunction(void);
		RAYTRACER_EXPORTS virtual ~TransferFunction(void);
		RAYTRACER_EXPORTS void addKey(TFKey key);
		RAYTRACER_EXPORTS Vec4d interpolateColor(double intensity); //TODO!
		RAYTRACER_EXPORTS void print();
	private:
		std::vector<TFKey> keys;
	};

}
