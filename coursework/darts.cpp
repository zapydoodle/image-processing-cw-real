/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - face.cpp
//
/////////////////////////////////////////////////////////////////////////////

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
	imwrite( "detected.jpg", frame );

	return 0;
}

std::vector<Rect>  HoughDetection(){

}

/** @function detectAndDisplay */
//also calculates iou tpr and f1 score
void detectAndDisplay( Mat frame )
{

	std::vector<Rect> faces;

    std::vector<std::vector<int>> faceLocations;
    faceLocations= {{97,79,162,158},{956,73,178,166}};

	Mat frame_gray;

	// 1. Prepare Image by turning it into Grayscale and normalising lighting
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// 2. Perform Viola-Jones Object Detection 
	cascade.detectMultiScale( frame_gray, faces, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(10, 10), Size(300,300) ); //was 300*300

       // 3. Print number of Faces found
	std::cout << faces.size() << std::endl;



    //IOU TPR AND F1
    //REMEMBER TO COPY OVER FACE LOCATIONS
    bool test=true;//set to false to ignore tests
    if (test==true){
        //Draw box around actual faces
        for( int i = 0; i < faceLocations.size(); i++ ){
            rectangle(frame, Point(faceLocations[i][0], faceLocations[i][1]), Point(faceLocations[i][0]+faceLocations[i][2], faceLocations[i][1]+faceLocations[i][3]), Scalar( 0,0 , 255 ), 2);
        }
        //IOU
        int amountOfFaces=faceLocations.size();
        std::vector<float> iouValues;
        float threshold=0.5;
        //create intersection rectangle and calculate area for each detected rectangle for each real rectangle
        //ignore iou of 0 (not matching rectangles)
        //
        for( int detectedFaceIndex = 0; detectedFaceIndex < faces.size(); detectedFaceIndex++ ){
            int detectedXLeft=faces[detectedFaceIndex].x;
            int detectedYLeft=faces[detectedFaceIndex].y;
            int detectedXRight=faces[detectedFaceIndex].x+faces[detectedFaceIndex].width;
            int detectedYRight=faces[detectedFaceIndex].y+faces[detectedFaceIndex].height;
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
        int falsePositive=faces.size()-iouValues.size();
        int falseNegative=faceLocations.size()-iouValues.size();
        float Precision=truePositive/float(truePositive+falsePositive);
        float Recall=truePositive/float(truePositive+falseNegative);
        float tpr= iouValues.size()/float(faceLocations.size()+iouValues.size());
        std::cout<<"tpr: "<<tpr<<std::endl;
        float f1Score=(Precision*Recall)/float(float(Precision+Recall)/2);
        std::cout<<"F1 Score: "<<f1Score<<std::endl;


    }
    // 4. Draw box around faces found
    for( int i = 0; i < faces.size(); i++ )
    {
        rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar( 0, 255, 0 ), 2);
    }


}
