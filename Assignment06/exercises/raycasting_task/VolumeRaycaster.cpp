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
    Vec3i dims = getVolumeDims();

    double val;
    for (int z = 0; z < dims.z(); z++)
    {   
        val = volume.F(x, y, z);

        if( val > isovalue) {
            return tf.interpolateColor(val);
        }
    }
    return tf.interpolateColor(0.0);
}
    
    Vec4d VolumeRaycaster::getMaximum(int x, int y)
    {
        //Task 1
        //Note: We use the same projection as in getAverage()
        Vec3i dims = getVolumeDims();
        
        double max = 0;
        for (int z = 0; z < dims.z(); z++)
        {
            const double val = volume.F(x, y, z);
            if(val > max)
                max = val;
        }
        
        return tf.interpolateColor(max);
    }
    
    Vec4d VolumeRaycaster::accummulateFrontToBack(int x, int y)
    {
        //Task 2
        //Note: We use the same projection as in getAverage()
        
        Vec3i dims = getVolumeDims();
        
        Vec4d cOut(0,0,0,0);
        double aOut = 0;
        
        // Cout = Cin + (1 - αin) αC
        // αout = αin + (1 - αin) α
        for (int z = 0; z < dims.z(); z++)
        {
            const double val = volume.F(x, y, z);
            Vec4d color = tf.interpolateColor(val);
            
            cOut = cOut + (1 - aOut) * color * color.a();
            aOut = (1 - aOut) * color.a() + aOut;
            cOut.a() = aOut;
            
            if(aOut >= 1){
                break;
            }
        }
        
        return cOut;
    }
    
    Vec4d VolumeRaycaster::accummulateBackToFront(int x, int y)
    {
        //Task 2
        //Note: We use the same projection as in getAverage()
        Vec3i dims = getVolumeDims();
        
        Vec4d cOut(0,0,0,0);
        double aOut = 0;
        
        // Cout = (1 - α) Cin + αC
        // αout = (1 - α) αin + α
        for (int z = dims.z()-1; z >= 0; z--)
        {
            const double val = volume.F(x, y, z);
            Vec4d color = tf.interpolateColor(val);
            
            cOut = (1 - color.a()) * cOut + color.a() * color;
            aOut = (1 - color.a()) * aOut + color.a();
            cOut.a() = aOut;
        }
        
        return cOut;
    }
}

