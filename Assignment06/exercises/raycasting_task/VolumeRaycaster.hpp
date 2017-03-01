#pragma once
#include <memory>
#include "Math.hpp"
#include "TransferFunction.hpp"
#include "Volume.hpp"
#include "Image.hpp"

namespace rt
{
class VolumeRaycaster
{
public:
    VolumeRaycaster(Volume& vol);
    virtual ~VolumeRaycaster();
    
	void renderToImage_FirstHit(const TransferFunction& argTF, double isovalue, std::shared_ptr<rt::Image> image);
    void renderToImage_Maximum(const TransferFunction& argTF, std::shared_ptr<rt::Image> image);
    void renderToImage_Average(const TransferFunction& argTF, std::shared_ptr<rt::Image> image);
    void renderToImage_AccumFrontToBack(const TransferFunction& argTF, std::shared_ptr<rt::Image> image);
    void renderToImage_AccumBackToFront(const TransferFunction& argTF, std::shared_ptr<rt::Image> image);

	Vec3i getVolumeDims() const;

private:
    Vec4d getFirstHit(int x, int y, double isovalue);
    Vec4d getMaximum(int x, int y);
    Vec4d getAverage(int x, int y);
    Vec4d accummulateFrontToBack(int x, int y);
    Vec4d accummulateBackToFront(int x, int y);
    Volume& volume;
    TransferFunction tf;
};
}

