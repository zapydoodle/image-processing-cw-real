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
#include <fstream>


using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame ,int threshold);

/** Global variables */
String cascade_name = "cascade.xml";
CascadeClassifier cascade;


/** @function main */
int main( int argc, const char** argv )
{
    int threshold=125;

    // 1. Read Input Image
    Mat frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // 2. Load the Strong Classifier in a structure called `Cascade'
    if( !cascade.load( cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if(argc==3){
        const char* thresholdArg;
        thresholdArg=argv[2];

        stringstream s(thresholdArg);
        s >> threshold;

    }

    // 3. Detect Faces and Display Result
    detectAndDisplay( frame,threshold );

    // 4. Save Result Image

    imwrite( "detected_hough.jpg", frame );


    return 0;
}


void detectAndDisplay( Mat frame,int threshold )
{


    //Preparing image and applying Sobel filter
    std::vector<Rect> darts;

    std::vector<std::vector<int>> faceLocations;
    faceLocations= {{97,79,162,158},{956,73,178,166}};
    Mat frameCopy=frame.clone();
    Mat frame_gaussian;
    Mat frame_gray;
    Mat frame_sobel;
    Mat frame_canny;
    Mat sobel_x;
    Mat sobel_y;
    Mat sobel_x_converted;
    Mat sobel_y_converted;
    Mat frame_sobel_old;
    GaussianBlur(frame, frame_gaussian, Size(3, 3), 0, 0, BORDER_DEFAULT);
    cvtColor( frame_gaussian, frame_gray, CV_BGR2GRAY );
    Sobel(frame_gray,sobel_x,CV_16S,1,0,3,1,0,BORDER_DEFAULT);
    Sobel(frame_gray,sobel_y,CV_16S,0,1,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(sobel_x, sobel_x_converted);
    convertScaleAbs(sobel_y,sobel_y_converted);
    addWeighted(sobel_x_converted, 0.5, sobel_y_converted, 0.5, 0, frame_sobel);//frame_sobel_old
    //Canny(frame, frame_sobel, 50, 200, 3);
    //imwrite( "oldsobel.jpg", frame_sobel_old );
    imwrite( "sobel.jpg", frame_sobel );



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
    int maxHough=0;


    std::vector<std::vector<int>> houghSpace((frameDiagonal*2)+1, std::vector<int>(theta, 0));
    //updating hough space where edge pixels are present
    //get pixel values int pixelGrayValue = (int)frame_sobel.at<uchar>(y,x);



    for(int y=0;y<frameHeight;y++){
        for(int x=0;x<frameWidth;x++){
            if((int)frame_sobel.at<uchar>(y,x)>220){
                for(int possibleThetas=0;possibleThetas<theta;possibleThetas++){
                    double thetaRadian= possibleThetas*(M_PI/180);
                    int corrospondingRho=(int)(((x-frameWidthMid)*cos(thetaRadian))+((y-frameHeightMid)*sin(thetaRadian)));//remove frameWidthMid

                    int RhoIndex=corrospondingRho+frameDiagonal;
                    houghSpace[RhoIndex][possibleThetas]+=1;
                    if(houghSpace[RhoIndex][possibleThetas]>maxHough){
                        maxHough=houghSpace[RhoIndex][possibleThetas];
                    }


                }
            }

        }
    }
    //write out houghspace here
    ofstream houghSpaceCSV;
    houghSpaceCSV.open("hough_space.txt",ofstream::out | ofstream::trunc);
    houghSpaceCSV.close();
    houghSpaceCSV.open("hough_space.txt");
    bool writeOut=true;
    if(writeOut){
        for(int y=0;y<houghSpace.size();y++){
            for(int x=0;x<houghSpace[y].size();x++){
                houghSpaceCSV<<houghSpace[y][x];
                houghSpaceCSV<<",";
            }
            houghSpaceCSV<<"\n";
        }
    }
    threshold=0.50*maxHough;

    std::vector<std::vector<double>>lines;
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
                int x1=(int) x+(1000*(-b));//removed 1000's
                int y1=(int)y+(1000*a);
                int x2=(int)x-(1000*(-b));
                int y2=(int)y -(1000*a);
                std::vector<int> pointsToAdd;
                pointsToAdd.push_back(x1);
                pointsToAdd.push_back(y1);
                pointsToAdd.push_back(x2);
                pointsToAdd.push_back(y2);
                lineCoordinates.push_back(pointsToAdd);

            }
        }

    }
    std::vector<std::vector<int>> intersectionPoints;//contains x,y coords of points and assigned weights
    //0=x, 1=y , 2=weight
    //weight is increased for every line that passes through point

    //find intersection points
    int maxWeight=0;
    for (int pointsIndex=0;pointsIndex<lineCoordinates.size()-1;pointsIndex++){
        int x1=lineCoordinates[pointsIndex][0];
        int y1=lineCoordinates[pointsIndex][1];
        int x2=lineCoordinates[pointsIndex][2];
        int y2=lineCoordinates[pointsIndex][3];

        for (int pointsIndexPlusOne=pointsIndex+1;pointsIndexPlusOne<lineCoordinates.size();pointsIndexPlusOne++){
            int x3=lineCoordinates[pointsIndexPlusOne][0];
            int y3=lineCoordinates[pointsIndexPlusOne][1];
            int x4=lineCoordinates[pointsIndexPlusOne][2];
            int y4=lineCoordinates[pointsIndexPlusOne][3];
            float denominator=((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4));

            if(denominator!=0){//
                float xNumerator=((((x1*y2)-(y1*x2))*(x3-x4))-((x1-x2)*((x3*y4)-(y3*x4))));
                float yNumerator=((((x1*y2)-(y1*x2))*(y3-y4))-((y1-y2)*((x3*y4)-(y3*x4))));
                float xFloat=xNumerator/denominator;
                float yFloat=yNumerator/denominator;
                int x=(int)xFloat;
                int y=(int)yFloat;
                bool pointAlreadyRegistered=false;
                for(int intersectionPointsIndex=0;intersectionPointsIndex<intersectionPoints.size();intersectionPointsIndex++){
                    if (((x<=intersectionPoints[intersectionPointsIndex][0]+3)&&(x>=intersectionPoints[intersectionPointsIndex][0]-3))
                    &&((y<=intersectionPoints[intersectionPointsIndex][1]+3)&&(y>=intersectionPoints[intersectionPointsIndex][1]-3))
                    &&((x<=frameWidth)&&(x>=0))&&((y<=frameHeight)&&y>=0)){
                        pointAlreadyRegistered=true;
                        intersectionPoints[intersectionPointsIndex][2]++;
                        if (intersectionPoints[intersectionPointsIndex][2]>maxWeight){
                            maxWeight=intersectionPoints[intersectionPointsIndex][2];
                        }
                    }
                }
                if(pointAlreadyRegistered==false){
                    std::vector<int> currentIntersectionPoint;
                    currentIntersectionPoint.push_back(x);
                    currentIntersectionPoint.push_back(y);
                    currentIntersectionPoint.push_back(1);
                    intersectionPoints.push_back(currentIntersectionPoint);
                }
            }

        }
    }
    //calculate which intersection points are in the centre of dartboard
    int weightThreshold=0.2*maxWeight;
    std::vector<std::vector<int>> dartboardCentre;
    for(int i=0;i< intersectionPoints.size();i++){
        //if(((intersectionPoints[i][0]>=1040)&&(intersectionPoints[i][0]<=1050))&&((intersectionPoints[i][1]>=150)&&(intersectionPoints[i][1]<=165))){
        //    std::cout<<intersectionPoints[i][0]<<" "<<intersectionPoints[i][1]<<" "<<intersectionPoints[i][2]<<"\n";
        //}
        if(intersectionPoints[i][2]>=weightThreshold){
            std::vector<int> currentDartboardCentre;
            currentDartboardCentre.push_back(intersectionPoints[i][0]);
            currentDartboardCentre.push_back(intersectionPoints[i][1]);
            dartboardCentre.push_back(currentDartboardCentre);

        }
    }

    //find dartboards with Viola Jones
    equalizeHist( frame_gray, frame_gray );
    cascade.detectMultiScale( frame_gray, darts, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(10, 10), Size(300,300) ); //was 300*300

    //combine Viola Jones and Hough Transform to draw boxes around dartboards
    std::vector<Rect> correctDarts;
    for(int dartboardCentreIndex=0; dartboardCentreIndex<dartboardCentre.size();dartboardCentreIndex++){
        int x=dartboardCentre[dartboardCentreIndex][0];
        int y=dartboardCentre[dartboardCentreIndex][1];
        bool enclosed=false;
        for(int rectangleIndex=0; rectangleIndex<darts.size();rectangleIndex++){
            if((x>=darts[rectangleIndex].x)&&(x<=darts[rectangleIndex].x+darts[rectangleIndex].width)
            &&(y>=darts[rectangleIndex].y)&&(y<=darts[rectangleIndex].y+darts[rectangleIndex].height)){
                correctDarts.push_back(darts[rectangleIndex]);
                enclosed=true;
            }
        }
    }
    //draw boxes around actual dartboards
    for( int i = 0; i < faceLocations.size(); i++ ){
        rectangle(frame, Point(faceLocations[i][0], faceLocations[i][1]), Point(faceLocations[i][0]+faceLocations[i][2], faceLocations[i][1]+faceLocations[i][3]), Scalar( 0,0 , 255 ), 2);
    }
    //draw boxes around found dartboards
    for( int i = 0; i < correctDarts.size(); i++ )
    {
        rectangle(frame, Point(correctDarts[i].x, correctDarts[i].y), Point(correctDarts[i].x + correctDarts[i].width,correctDarts[i].y + correctDarts[i].height), Scalar( 0, 255, 0 ), 2);
    }




    //calculate fpr and f1 score
    bool test=true;//set to false to ignore tests
    if (test==true){
        //IOU
        int amountOfFaces=faceLocations.size();
        std::vector<float> iouValues;
        float threshold=0.55;
        //create intersection rectangle and calculate area for each detected rectangle for each real rectangle
        //ignore iou of 0 (not matching rectangles)
        //
        for( int detectedFaceIndex = 0; detectedFaceIndex < correctDarts.size(); detectedFaceIndex++ ){
            int detectedXLeft=correctDarts[detectedFaceIndex].x;
            int detectedYLeft=correctDarts[detectedFaceIndex].y;
            int detectedXRight=correctDarts[detectedFaceIndex].x+correctDarts[detectedFaceIndex].width;
            int detectedYRight=correctDarts[detectedFaceIndex].y+correctDarts[detectedFaceIndex].height;
            int detectedArea=(detectedXRight-detectedXLeft+1)*(detectedYRight-detectedYLeft+1);

            for( int  realFaceIndex= 0; realFaceIndex < faceLocations.size(); realFaceIndex++ ){
                int realXLeft=faceLocations[realFaceIndex][0];
                int realYLeft=faceLocations[realFaceIndex][1];
                int realXRight=faceLocations[realFaceIndex][0]+faceLocations[realFaceIndex][2];
                int realYRight=faceLocations[realFaceIndex][1]+faceLocations[realFaceIndex][3];

                int intersectionXLeft = max(detectedXLeft, realXLeft);
                int intersectionYLeft = max(detectedYLeft, realYLeft);
                int intersectionXRight = min(detectedXRight, realXRight);
                int intersectionYRight = min(detectedYRight, realYRight);

                int intersectionXlength=intersectionXRight-intersectionXLeft+1;
                int intersectionYlength=intersectionYRight-intersectionYLeft+1;
                //std::cout<< "x length:"<< intersectionXlength<<"y length"<<intersectionYlength<<std::endl;
                if (intersectionXlength<=0){
                    intersectionXlength=0;
                }
                if (intersectionYlength<=0){
                    intersectionYlength=0;
                }

                int intersectionArea=intersectionXlength *intersectionYlength;

                int realArea=(realXRight-realXLeft+1)*(realYRight-realYLeft+1);

                float iou=intersectionArea/float( detectedArea+realArea-intersectionArea);

                if (iou>=threshold){
                    iouValues.push_back(iou);
                }

            }
        }

        int truePositive=iouValues.size();
        int falsePositive=correctDarts.size()-iouValues.size();
        int falseNegative=faceLocations.size()-iouValues.size();
        float Precision=truePositive/float(truePositive+falsePositive);
        float Recall=truePositive/float(truePositive+falseNegative);
        float tpr= truePositive/float(truePositive+falseNegative);
        std::cout<<"tpr: "<<tpr<<std::endl;
        float f1Score=2*(Precision*Recall)/float(float(Precision+Recall));
        std::cout<<"F1 Score: "<<f1Score<<std::endl;
        //std::cout<<faceLocations.size()<<" "<<iouValues.size()<<"\n";
    }



    Size frame1Size=frame.size();


}

















