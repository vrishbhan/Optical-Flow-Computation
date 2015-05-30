///////////////////////////////////////////////////////////////////////////////////
//  Optical Flow Calculation
//  Created by - Vrishbhan Singh Sisodia
//  San Jose State University
//  Version - 1.0
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define IMG_ROW 1280//1920
#define IMG_COL 720//1080
#define BLK_SIZE 16
#define EXT 8

void detection( Mat , Mat);

int main()
{
    
    // Images to capture the frame from video or camera or from file
    Mat curr_frame, prev_frame ;
    //Mat curr_frame = imread('1.png');//Input from image file
    //Mat prev_frame = imread('2.png');
    //VideoCapture cap("1.mp4"); // open video from file
    VideoCapture cap(0); // open the default camera
    
    cap.set(CV_CAP_PROP_FRAME_WIDTH,IMG_ROW);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,IMG_COL);

    // Find whether to detect the object from file or from camera.
    if(!cap.isOpened())  // check if we succeeded
        return -1; 

    // Capture from the camera.
    for(;;)
    {
        // Wait for a while before proceeding to the next frame
        if( cvWaitKey(1) >= 0 )
            break; 
        cap >> prev_frame; // get a new frame from camera
        cap >> curr_frame; // get a new frame from camera  
        detection( curr_frame, prev_frame );   
    }
    // return 0 to indicate successful execution of the program
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
/*
Name - Detection 
Parameters Taken - Current Frame(Color), Previous Frame(Color)
Description - Takes Current Frame and Previous Frame as input and Calculates
    Motion Vectors using full search SAD (Sum of Absolute Difference)
    based Block Matching Algorithm
*/
///////////////////////////////////////////////////////////////////////////////////
void detection( Mat current, Mat previous)
{

    Mat A, B;
    Mat disp_curr = current;
    cvtColor( current, A, CV_BGR2GRAY );
    cvtColor( previous, B, CV_BGR2GRAY );
    //imshow( "CURR_FRAME_Gray", A );
    //imshow( "PREV_FRAME_Gray", B );   

    int dx[IMG_ROW/BLK_SIZE][IMG_COL/BLK_SIZE],dy[IMG_ROW/BLK_SIZE][IMG_COL/BLK_SIZE],SAD[IMG_ROW/BLK_SIZE][IMG_COL/BLK_SIZE];
    int X;
    int l,p;

    l=-1;
    for(int i=0;i<=IMG_ROW-BLK_SIZE;i=i+BLK_SIZE) 
    {
        p=-1;
        l=l+1;
        for (int j=0; j<=IMG_COL-BLK_SIZE; j=j+BLK_SIZE)
        {
            p=p+1;
            SAD[l][p]=99999;
            for (int m=-EXT; m<=EXT; m=m+8)
            {
                for (int n=-EXT; n<=EXT; n=n+8)
                {
                    if ((i+m)>=0 && (i+m)<=IMG_ROW-BLK_SIZE && (j+n)>=0 && (j+n)<=IMG_COL-BLK_SIZE)
                    {
                        X=0;
                        for (int h=0; h < BLK_SIZE ; h++)                           
                        {
                            for (int k =0; k < BLK_SIZE; k++)
                            {                         
                               X += abs(B.at<uchar>((j+n+k),(i+m+h))-A.at<uchar>((j+k),(i+h)));    
                            }
                        }
                        if (X < SAD[l][p])
                        {
                           SAD[l][p]=X;
                            dx[l][p]=m;
                            dy[l][p]=n;
                        }
                    }
                }
            }
            arrowedLine(disp_curr, Point(i+(dx[l][p]),j+(dy[l][p])), Point(i,j), Scalar(0, 255, 0), 1, 8, 0, 0.1);
        }      
    }
   imshow("Motion Vectors",disp_curr);
}