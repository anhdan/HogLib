/* GRAY PATCH LAYOUT
 *
                        Width
              +-----+-----+-----+-----+-----+
              |cell |     |     |     |     |
              |     |     |     |     |     |
              +-----+-----+-----+-----+-----+
              |     |     |           |     |
              |     |     |           |     |
              +-----+-----+ block(i,j)+-----+  ^
    Height    |     |     |           |     |  |
              |     |     |           |     |  |
              +-----+-----+-----+-----+-----+  | block
              |     |     |     |     |     |  |
              |     |     |     |     |     |  |
              +-----+-----+-----+-----+-----+  v
                                <----------->
 */



/* FEATURE MAP LAYOUT

               (Width/cell_size -2) * (Height/cell_size -2)
              +----------...-----------+-------...----------+
              |          ...           |       ...          |
              |          ...           |       ...          |
  Feature     .                        .                    .
   Number     .                        .                    .
              .                        .                    .
              +----------...-----------+-------...----------+
                                       |
                         cullumn [i * block_width + j]

 * NOTE: HOG vector of block [i, j] is correspond to the cullumn vector [i * block_width + j] of the feature map
 *
 */

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv/cv.h>

#include "HOGLib/fhog.hpp"

using namespace std;

#define     CELL_SIZE       4


int main( int argc, char **argv )
{
    //===== 1. Reading image and getting image patch
    assert( argc == 2 );
    cv::Mat img = cv::imread( argv[1] );
    cv::Mat gray;
    if( img.channels() == 3 )
    {
        cv::cvtColor( img, gray, cv::COLOR_RGB2GRAY );
    }
    else
    {
        img.copyTo( gray );
    }


    cv::Rect rect;
    rect.x = 100;
    rect.y = 100;
    rect.width  = 96;
    rect.height = 96;
    cv::Mat patch = gray( rect );


    //===== 2. Extract HOG feature
    IplImage iplPatch = patch;      // IplImage la mot kieu bieu dien khac cua anh, dung trong C (cv::Mat dung trong C++)
    CvLSVMFeatureMapCaskade *map;

    getFeatureMaps( &iplPatch, CELL_SIZE, &map );

    normalizeAndTruncate( map, 0.2f );  // Chuan hoa gia tri cua cac vector feature

    PCAFeatureMaps( map );              // Ham dung de giam so chieu vector feature (VD: giam feature vector length tu 108 -> 31)

    cv::Mat featureMap = cv::Mat( map->sizeX * map->sizeY, map->numFeatures, CV_32F, map->map );
    featureMap = featureMap.t();

    printf("Image patch size in pixel: [%d x %d]\n", rect.width, rect.height );
    printf("Image patch size in cell : [%d x %d]\n", rect.width/CELL_SIZE, rect.height/CELL_SIZE );
    printf("map size: [%d x %d x%d]\n", map->sizeX, map->sizeY, map->numFeatures );
    printf("Feature map size: [%d x %d]\n", featureMap.cols, featureMap.rows );

    cv::imshow( "Image patch", patch );
    cv::waitKey( 0 );

    freeFeatureMapObject( &map );

    return 0;
}

