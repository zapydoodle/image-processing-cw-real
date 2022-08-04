// header inclusion
#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string>


using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String cascade_name = "cascade.xml";
CascadeClassifier cascade;


/** @function main */
int main( int argc, const char** argv )
{
    // 1. Read Input Image
    Mat frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);


    // 2. Load the Strong Classifier in a structure called `Cascade'
    if( !cascade.load( cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

    // 3. Detect Faces and Display Result
    detectAndDisplay( frame );

    // 4. Save Result Image
    //UNCOMMENT THIS!
    //imwrite( "detected.jpg", frame );


    return 0;
}


void detectAndDisplay( Mat frame )
{
    //Preparing image and applying Sobel filter
    std::vector<Rect> faces;

    std::vector<std::vector<int>> faceLocations;
    faceLocations= {{155,96,259,263}};

    Mat frame_gaussian;
    Mat frame_gray;
    Mat frame_sobel;
    Mat sobel_x;
    Mat sobel_y;
    Mat sobel_x_converted;
    Mat sobel_y_converted;
    GaussianBlur(frame, frame_gaussian, Size(3, 3), 0, 0, BORDER_DEFAULT);
    cvtColor( frame_gaussian, frame_gray, CV_BGR2GRAY );
    Sobel(frame_gray,sobel_x,CV_16S,1,0,3,1,0,BORDER_DEFAULT);
    Sobel(frame_gray,sobel_y,CV_16S,0,1,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(sobel_x, sobel_x_converted);
    convertScaleAbs(sobel_y,sobel_y_converted);
    addWeighted(sobel_x_converted, 0.5, sobel_y_converted, 0.5, 0, frame_sobel);
    imwrite( "sobel.jpg", frame_sobel );

    //Applying hough transform and getting points

    std::vector<std::vector<int>> lineCoordinates;
    //creating empty hough space
    Size frameSize=frame_sobel.size();
    double frameHeight=frameSize.height;
    double frameWidth=frameSize.width;
    double frameDiagonal=sqrt((frameWidth*frameWidth)+(frameHeight*frameHeight));
    double theta=180;
    double thetaStep=1;
    double rhoStep=((2*frameDiagonal)/180);
    std::vector<double> thetas;
    std::vector<double> rhos;
    for(double counter=0;counter<theta;counter+=thetaStep){

        thetas.push_back(counter);
    }
    for(double counter=-frameDiagonal;counter<frameDiagonal;counter+=rhoStep){

        rhos.push_back(counter);
    }
    std::cout<<rhos.size()<<"\n";
    int rhosSize=rhos.size();
    int thetasSize=thetas.size();
    std::vector<std::vector<int>> houghSpace(rhosSize, std::vector<int>(thetasSize, 0));
    //setting edge pixes in hough space



















}



