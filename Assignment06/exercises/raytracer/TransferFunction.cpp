#include "TransferFunction.hpp"

namespace rt{

	TransferFunction::TransferFunction(void)
	{
	}

	TransferFunction::~TransferFunction(void)
	{
	}

	bool sortKeys(TFKey k1, TFKey k2){
		if (k1.intensity < k2.intensity)
			return true;
		return false;
	}

	void TransferFunction::addKey(TFKey key){
		keys.push_back(key);
		sort(keys.begin(), keys.end(), sortKeys);
	}

	void TransferFunction::print(){
		printf("\n --- Printing Transfer Function --- \n");
		for (size_t i = 0; i < keys.size(); i++){
			printf("\nKey #%d : Intensity %g Color (%g %g %g %g)", i, keys[i].intensity, keys[i].color[0], keys[i].color[1], keys[i].color[2], keys[i].color[3]);
		}
		printf("\n");
	}

	Vec4d TransferFunction::interpolateColor(double intensity){
		//TODO!

		if (intensity > 1.0 || intensity < 0.0){
			printf("Unnormalized intensity!\n");
		}

		Vec4d col(0, 0, 0, 0);

		//if nothing to interpolate return 0
		if (keys.size() < 2){
			return col;
		}

		for (int i = 0; i < (int)keys.size() - 1; i++){
			if (intensity >= keys[i].intensity && intensity <= keys[i + 1].intensity){
				double L = intensity - keys[i].intensity;
				double R = keys[i + 1].intensity - intensity;
				double tot = L + R;
				L /= tot;
				R /= tot;

				return keys[i].color * R + keys[i + 1].color * L;
			}
		}
		return col;

	}

}