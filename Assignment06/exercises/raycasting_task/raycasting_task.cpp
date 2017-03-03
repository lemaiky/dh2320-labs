#include <raytracer/Image.hpp>
#include <raytracer/Math.hpp>
#include <opengl/RaytracerWindow.hpp>
#include <raytracer/Volume.hpp>
#include <raytracer/AmiraMeshReader.h>
#include "VolumeRaycaster.hpp"

std::string gDataPath= ""; ///< The path pointing to the resources (OBJ, shader)

int Task1(rt::VolumeRaycaster& vraycaster)
{
    //Gray Ramp Transfer function
    rt::TransferFunction tf;
    tf.addKey(rt::TFKey(0, rt::Vec4d(0, 0, 0, 0)));
    tf.addKey(rt::TFKey(1, rt::Vec4d(1, 1, 1, 1)));
    tf.print();

    //Create output image
    rt::Vec3i dims = vraycaster.getVolumeDims();
    std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(dims.x(), dims.y());

    //////////////////////////////////////////////////////////////////
    
    //Call appropriate render function: Change this!
    //vraycaster.renderToImage_Average(tf, image);
    vraycaster.renderToImage_FirstHit(tf, 0.65, image);
    //vraycaster.renderToImage_Maximum(tf, image);

    //////////////////////////////////////////////////////////////////

    //Show
    rt::RaytracerWindow win(dims.x(), dims.y());
    if(win.init())
        win.drawOnce(image);
    else
        return -1;

    return 0;
}


int Task2(rt::VolumeRaycaster& vraycaster)
{
    //Gray Ramp Transfer function
    rt::TransferFunction tf;
    tf.addKey(rt::TFKey(0, rt::Vec4d(0, 0, 0, 0)));
    tf.addKey(rt::TFKey(1, rt::Vec4d(1, 1, 1, 1)));
    tf.print();

    //Create output image
    rt::Vec3i dims = vraycaster.getVolumeDims();
    std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(dims.x(), dims.y());

    //////////////////////////////////////////////////////////////////
    
    //Call appropriate render function: Change this!
    //vraycaster.renderToImage_AccumBackToFront(tf, image);
    vraycaster.renderToImage_AccumFrontToBack(tf, image);

    //////////////////////////////////////////////////////////////////

    //Show
    rt::RaytracerWindow win(dims.x(), dims.y());
    if(win.init())
        win.drawOnce(image);
    else
        return -1;

    return 0;
}


int Task2_Test(rt::VolumeRaycaster& vraycaster)
{
    //Gray Ramp Transfer function
    rt::TransferFunction tf;
    tf.addKey(rt::TFKey(0, rt::Vec4d(0, 0, 0, 0)));
    tf.addKey(rt::TFKey(1, rt::Vec4d(1, 1, 1, 1)));
    tf.print();

    //Create output image
    rt::Vec3i dims = vraycaster.getVolumeDims();
    std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(dims.x(), dims.y());
    std::shared_ptr<rt::Image> imageB2F = std::make_shared<rt::Image>(dims.x(), dims.y());
    std::shared_ptr<rt::Image> imageF2B = std::make_shared<rt::Image>(dims.x(), dims.y());

    vraycaster.renderToImage_AccumFrontToBack(tf, imageF2B);
    vraycaster.renderToImage_AccumBackToFront(tf, imageB2F);

    //Compare both results
    rt::Vec4d GlobalDifference(0,0,0,0);
    for (int y = 0; y<(int)(imageB2F->height()); y++)
    {
        for (int x = 0; x<(int)(imageB2F->width()); x++)
        {
            rt::Vec4d LocalDifference = imageB2F->pixel(x, y) - imageF2B->pixel(x, y);
            LocalDifference[0] = fabs(LocalDifference[0]);
            LocalDifference[1] = fabs(LocalDifference[1]);
            LocalDifference[2] = fabs(LocalDifference[2]);
            LocalDifference[3] = fabs(LocalDifference[3]);
            image->setPixel(LocalDifference, x, y);
            GlobalDifference += LocalDifference;
        }
    }

    printf("GlobalDifference = (%g, %g, %g, %g)\n",
            GlobalDifference.r(), GlobalDifference.g(), GlobalDifference.b(), GlobalDifference.a());

    //Show
    rt::RaytracerWindow win(dims.x(), dims.y());
    if(win.init())
        win.drawOnce(image);
    else
        return -1;

    return 0;
}

int Task3(rt::VolumeRaycaster& vraycaster)
{
    //////////////////////////////////////////////////////////////////

    rt::TransferFunction tf;
    
    //Change this: Create a new transfer function by adding more keys!
    //A transfer function key has two values:
    // - The first value is the data value. It ranges from 0 to 1.
    // - The second value is the color and opacity. The components range from 0 to 1.
    tf.addKey(rt::TFKey(0, rt::Vec4d(0, 0, 0, 0)));
    
    tf.addKey(rt::TFKey(0.35, rt::Vec4d(1, 1, 1, 0)));
    tf.addKey(rt::TFKey(0.4, rt::Vec4d(1, 1, 1, 0.1)));
    tf.addKey(rt::TFKey(0.45, rt::Vec4d(1, 1, 1, 0)));
    
    tf.addKey(rt::TFKey(0.80, rt::Vec4d(0, 1, 1, 0.0)));
    tf.addKey(rt::TFKey(0.65, rt::Vec4d(0, 0.8, 1, 0.6)));
    tf.addKey(rt::TFKey(0.60, rt::Vec4d(0, 0, 1, 0.0)));

    tf.addKey(rt::TFKey(1, rt::Vec4d(0, 0, 0, 0)));

    tf.print();

    //////////////////////////////////////////////////////////////////

    //Create output image
    rt::Vec3i dims = vraycaster.getVolumeDims();
    std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(dims.x(), dims.y());

    //Call render function
    vraycaster.renderToImage_AccumBackToFront(tf, image);

    //Show
    rt::RaytracerWindow win(dims.x(), dims.y());
    if(win.init())
        win.drawOnce(image);
    else
        return -1;

    return 0;
}


int main(int argc, char** argv)
{
    std::string pathToThisFile=__FILE__;

    if(gDataPath.empty())
        gDataPath=rt::Math::getParentDirectoryFromFilePath(pathToThisFile);
    std::cerr<<"Using data path: "<<gDataPath<<std::endl;

    //Read Volume from an AmiraMesh file
    rt::Volume vol;
    readFile(gDataPath + "assets/stent16_cutted.am", vol);

    //Assign the volume to the VolumeRaycaster
    rt::VolumeRaycaster vraycaster(vol);

    //////////////////////////////////////////////////////////////////

    //Call the respective task
    //return Task1(vraycaster);
    //return Task2(vraycaster);
    //return Task2_Test(vraycaster);
    return Task3(vraycaster);

    //////////////////////////////////////////////////////////////////
}
