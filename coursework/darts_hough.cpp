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
#include <math.h>


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
    Mat frameCopy=frame.clone();
    Mat frame_gaussian;
    Mat frame_gray;
    Mat frame_sobel;
    Mat sobel_x;
    Mat sobel_y;
    Mat sobel_x_converted;
    Mat sobel_y_converted;
    Mat frame_canny;
    GaussianBlur(frame, frame_gaussian, Size(3, 3), 0, 0, BORDER_DEFAULT);
    cvtColor( frame_gaussian, frame_gray, CV_BGR2GRAY );
    Sobel(frame_gray,sobel_x,CV_16S,1,0,3,1,0,BORDER_DEFAULT);
    Sobel(frame_gray,sobel_y,CV_16S,0,1,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(sobel_x, sobel_x_converted);
    convertScaleAbs(sobel_y,sobel_y_converted);
    addWeighted(sobel_x_converted, 0.5, sobel_y_converted, 0.5, 0, frame_sobel);
    Canny(frame, frame_canny, 50, 200, 3);
    imwrite( "sobel.jpg", frame_canny );

    //Applying hough transform and getting points

    std::vector<std::vector<int>> lineCoordinates;
    //creating empty hough space
    Size frameSize=frame_sobel.size();
    int frameHeight=frameSize.height;
    int frameWidth=frameSize.width;
    int frameHeightMid=frameHeight/2;
    int frameWidthMid=frameWidth/2;
    int frameDiagonal=sqrt((frameWidth*frameWidth)+(frameHeight*frameHeight));
    int theta=180;


    std::vector<std::vector<int>> houghSpace((frameDiagonal*2)+1, std::vector<int>(theta, 0));
    //updating hough space where edge pixels are present
    //get pixel values int pixelGrayValue = (int)frame_sobel.at<uchar>(y,x);

    int highVal=0;

    for(int y=0;y<frameHeight;y++){
        for(int x=0;x<frameWidth;x++){
            if((int)frame_canny.at<uchar>(y,x)>0){
                for(int possibleThetas=0;possibleThetas<theta;possibleThetas++){
                    double thetaRadian= possibleThetas*(M_PI/180);
                    int corrospondingRho=(int)(((x-frameWidthMid)*cos(thetaRadian))+((y-frameHeightMid)*sin(thetaRadian)));//removd frameWidthMid

                    int RhoIndex=corrospondingRho+frameDiagonal;
                    houghSpace[RhoIndex][possibleThetas]+=1;
                    if(houghSpace[RhoIndex][possibleThetas]>highVal){
                        highVal=houghSpace[RhoIndex][possibleThetas];
                    }

                    //if((corrospondingRho>0)&&corrospondingRho<frameDiagonal*2){
//
                    //    houghSpace[corrospondingRho][possibleThetas]++;
                    //}
                }
            }

        }
    }
    //write out houghspace here
    bool writeOut=true;
    if(writeOut){
        for(int y=0;y<houghSpace.size();y++){
            for(int x=0;x<houghSpace[y].size();x++){
                if (houghSpace[y][x]>0){
                    //std::cout<<houghSpace[y][x]<<"\n";
                }
            }
        }
    }

    int threshold=highVal*0.4;
    std::cout<<highVal<<"\n";
    //find lines from hough space
    for(int y=0;y< houghSpace.size();y++){
        for(int x=0;x<houghSpace[y].size();x++){

            if (houghSpace[y][x]>=threshold){
                int rho=y-frameDiagonal;

                int theta=x;
                double a= cos(theta*(M_PI/180));
                double b= sin(theta*(M_PI/180));
                int x=(a*rho)+frameWidthMid;
                int y=(b*rho)+frameHeightMid;
                //get points on line
                int x1=(int) x+(100*(-b));//removed 1000's
                int y1=(int)y+(100*a);
                int x2=(int)x-(100*(-b));
                int y2=(int)y -(100*a);
                std::vector<int> pointsToAdd;
                pointsToAdd.push_back(x1);
                pointsToAdd.push_back(y1);
                pointsToAdd.push_back(x2);
                pointsToAdd.push_back(y2);
                lineCoordinates.push_back(pointsToAdd);
            }
        }

    }
    for(int i=0;i<lineCoordinates.size();i++){
        Point point1=Point(lineCoordinates[i][0],lineCoordinates[i][1]);
        Point point2=Point(lineCoordinates[i][2],lineCoordinates[i][3]);
        line(frame,point1,point2,Scalar(0,255,0),1);
        line(frame_canny,point1,point2,Scalar(255,255,255),1);

    }


    //line(frameCopy,Point(10,40),Point(300,350),Scalar(255,0,0),2);
    imwrite( "framelines.jpg", frame );
    imwrite( "framelinessobel.jpg", frame_canny);
    Size frame1Size=frame.size();


}

















