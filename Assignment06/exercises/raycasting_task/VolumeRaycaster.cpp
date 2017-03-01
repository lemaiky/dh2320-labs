#include "VolumeRaycaster.hpp"

namespace rt
{

VolumeRaycaster::VolumeRaycaster(Volume& volume_)
    :volume(volume_)
{
    volume.normalize();
}


VolumeRaycaster::~VolumeRaycaster()
{
}


Vec3i VolumeRaycaster::getVolumeDims() const
{
    return volume.getVolumeDims();
}


void VolumeRaycaster::renderToImage_FirstHit(const TransferFunction& argTF, double isovalue, std::shared_ptr<rt::Image> image)
{
    tf = argTF;
    #pragma omp parallel for
    for (int y = 0; y<(int)(image->height()); y++)
    {
        for (int x = 0; x<(int)(image->width()); x++)
        {
            Vec4d color = getFirstHit(x, y, isovalue);
            image->setPixel(color, x, y);
        }
    }
}


void VolumeRaycaster::renderToImage_Maximum(const TransferFunction& argTF, std::shared_ptr<rt::Image> image)
{
    tf = argTF;
    #pragma omp parallel for
    for (int y = 0; y<(int)(image->height()); y++)
    {
        for (int x = 0; x<(int)(image->width()); x++)
        {
            Vec4d color = getMaximum(x, y);
            image->setPixel(color, x, y);
        }
    }
}


void VolumeRaycaster::renderToImage_Average(const TransferFunction& argTF, std::shared_ptr<rt::Image> image)
{
    tf = argTF;
    #pragma omp parallel for
    for (int y = 0; y<(int)(image->height()); y++)
    {
        for (int x = 0; x<(int)(image->width()); x++)
        {
            Vec4d color = getAverage(x, y);
            image->setPixel(color, x, y);
        }
    }
}


void VolumeRaycaster::renderToImage_AccumFrontToBack(const TransferFunction& argTF, std::shared_ptr<rt::Image> image)
{
    tf = argTF;
    #pragma omp parallel for
    for (int y = 0; y<(int)(image->height()); y++)
    {
        for (int x = 0; x<(int)(image->width()); x++)
        {
            Vec4d color = accummulateFrontToBack(x, y);
            image->setPixel(color, x, y);
        }
    }
}


void VolumeRaycaster::renderToImage_AccumBackToFront(const TransferFunction& argTF, std::shared_ptr<rt::Image> image)
{
    tf = argTF;
    #pragma omp parallel for
    for (int y = 0; y<(int)(image->height()); y++)
    {
        for (int x = 0; x<(int)(image->width()); x++)
        {
            Vec4d color = accummulateBackToFront(x, y);
            image->setPixel(color, x, y);
        }
    }
}




Vec4d VolumeRaycaster::getAverage(int x, int y)
{
    //This is the number of vertices in the volume.
    // We need it to know the number of vertices in z-direction for an (x,y)-pair
    Vec3i dims = getVolumeDims();

    //Sum up the data value along our viewing ray.
    //We use an orthographic projection along the z-axis.
    //This means, we just need to run over all vertices in z-direction for an (x,y)-pair.
    double sum = 0;
    for (int z = 0; z < dims.z(); z++)
    {
        const double val = volume.F(x, y, z);
        sum += val;
    }

    //Compute the average
    const double avg = sum / dims.z();

    //Lookup the color in the transfer function, and return it
    return tf.interpolateColor(avg);
}


Vec4d VolumeRaycaster::getFirstHit(int x, int y, double isovalue)
{
    //Task 1
    //Note: We use the same projection as in getAverage()

    return Vec4d(0, 0, 0, 0);
}

Vec4d VolumeRaycaster::getMaximum(int x, int y)
{
    //Task 1
    //Note: We use the same projection as in getAverage()

    return Vec4d(0, 0, 0, 0);
}

Vec4d VolumeRaycaster::accummulateFrontToBack(int x, int y)
{
    //Task 2
    //Note: We use the same projection as in getAverage()

    return Vec4d(0, 0, 0, 0);
}

Vec4d VolumeRaycaster::accummulateBackToFront(int x, int y)
{
    //Task 2
    //Note: We use the same projection as in getAverage()

    return Vec4d(0, 0, 0, 0);
}

}
