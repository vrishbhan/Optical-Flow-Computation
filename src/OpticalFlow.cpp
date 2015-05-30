///////////////////////////////////////////////////////////////////////////////////
//  Optical Flow Calculation
//  Created by - Vrishbhan Singh Sisodia
//  San Jose State University
//  Version - 2.0
//	Adding Corner Detection 
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "cvaux.h"

using namespace std;
using namespace cv;
/*
#define IMG_WIDTH 1920 //Full HD 1080p
#define IMG_HEIGHT 1080
*/
#define IMG_WIDTH 1280 //HD 720p
#define IMG_HEIGHT 720
#define BLK_SIZE 16
#define EXT 8

/// Global variables
// Images to capture the frame from video or camera or from file
Mat curr_frame, prev_frame, curr_gray, prev_gray ;
int thresh = 10;

Mat mask;
void arrow_mask(int ,int , int , int);

/// Function header
void Flow( int, void* );

/** @function main */
int main()
{
    //VideoCapture cap("1.mov"); // open video from file
    VideoCapture cap(0); // open the default camera
    
    cap.set(CV_CAP_PROP_FRAME_WIDTH,IMG_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,IMG_HEIGHT);

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
   		cvtColor( curr_frame, curr_gray, CV_BGR2GRAY );
   		cvtColor( prev_frame, prev_gray, CV_BGR2GRAY );
        Flow( 0, 0 );
    }
    // return 0 to indicate successful execution of the program
    return 0;
}

/** @function Flow */
void Flow( int, void* )
{

  int dx[IMG_WIDTH/BLK_SIZE][IMG_HEIGHT/BLK_SIZE],dy[IMG_WIDTH/BLK_SIZE][IMG_HEIGHT/BLK_SIZE],SAD[IMG_WIDTH/BLK_SIZE][IMG_HEIGHT/BLK_SIZE];
  int X;
  int l,p;
  Mat dst;
  
  dst = curr_frame;
  mask = curr_gray;
  //FAST Corner Detection
  std::vector<cv::KeyPoint> keypoints1;
  std::vector<cv::KeyPoint> keypoints2;  

  FAST(curr_gray, keypoints1, thresh,true);
  for(size_t fc = 0; fc < keypoints1.size(); ++fc)
  {
    /// SAD based Block Matching Calculation
    l=-1;
    //Traversing Row of the Image for Block Selection
    for(int j=0; j<=IMG_HEIGHT-BLK_SIZE; j=j+BLK_SIZE)
    {
        p=-1;
        l=l+1;
        ///Checking Condition if any detected corner keypoint lies in the Selected ROW Block
        if (keypoints1[fc].pt.y>=j && keypoints1[fc].pt.y<j+BLK_SIZE)
        {
    		//Traversing Column of the Image for Block Selection
        	for (int i=0;i<=IMG_WIDTH-BLK_SIZE;i=i+BLK_SIZE) 
        	{
        		p=p+1;
        		///Checking Condition if any detected corner keypoint lies in the selected Block
        		if (keypoints1[fc].pt.x>=i && keypoints1[fc].pt.x<i+BLK_SIZE)
        		{
        	    	SAD[l][p]=99999; //Initializing to an arbitary high value
        	    	for (int m=-EXT; m<=EXT; m=m+8)
        	    	{
        	    	    for (int n=-EXT; n<=EXT; n=n+8)
        	    	    {
        	    	        if ((i+m)>=0 && (i+m)<=IMG_WIDTH-BLK_SIZE && (j+n)>=0 && (j+n)<=IMG_HEIGHT-BLK_SIZE)
        	    	        {
        	    	            X=0;
        	    	            for (int h=0; h < BLK_SIZE ; h++)                           
        	    	            {
        	    	                for (int k =0; k < BLK_SIZE; k++)
        	    	                {                         
        	    	                   X += abs(prev_gray.at<uchar>((j+n+k),(i+m+h))-curr_gray.at<uchar>((j+k),(i+h)));    
        	    	                }
        	    	            }
        	    	            //Comparing if X is less than the existing SAD, then save the value and location of X
        	    	            if (X < SAD[l][p])
        	    	            {
        	    	               SAD[l][p]=X;
        	    	                dx[l][p]=m;
        	    	                dy[l][p]=n;
        	    	            }
        	    	        }
        	    	    }
        	    	}
        	    	///Draw Motion Vectors to Show motion in the two frames
        	    	//arrowedLine(dst, Point(i+(dx[l][p]),j+(dy[l][p])), Point(i,j), Scalar(0, 255, 0), 1, 8, 0, 0.1); //Using OpenCV function
                arrow_mask(dx[l][p],dy[l][p],j,i); //By creating mask
                    break;
        		}
        	}     
            break; 
        }
    }
    ///Use to Draw the corner points
    //circle(dst, keypoints1[fc].pt, keypoints1[fc].size/3, CV_RGB(255, 0, 0));       
  }

  /// Showing the result 
  imshow( "Optical Flow", mask );//if using arrow_mask function for drawing arrows - in grayscale
  //imshow( "Optical Flow", dst ); // if using arrowedLine function of OpenCV - in color
}

void arrow_mask(int dx,int dy, int i, int j){
  if (dx == -EXT)
  {
    if (dy == -EXT)//topleft
    {
      mask.at<uchar>(i-1,j) = 200;
      mask.at<uchar>(i,j-1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i-ptr,j-ptr)= 200;
      }
    }
    else if (dy == 0)//centerleft
    {
      mask.at<uchar>(i-1,j-1) = 200;
      mask.at<uchar>(i+1,j-1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i,j-ptr)= 200;
      }
    }
    else if (dy == EXT)//bottomleft
    {
      mask.at<uchar>(i+1,j) = 200;
      mask.at<uchar>(i,j-1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i+ptr,j-ptr)= 200;
      }
    }
  }
  else if (dx == 0)
  {
    if (dy == -EXT)//topcenter
    {
      mask.at<uchar>(i-1,j+1) = 200;
      mask.at<uchar>(i-1,j-1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i-ptr,j)= 200;
      }
    }
    else if (dy == EXT)//bottomcenter
    {
      mask.at<uchar>(i+1,j+1) = 200;
      mask.at<uchar>(i+1,j-1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i+ptr,j)= 200;
      }
    }
  }
  else if (dx == EXT)
  {
    if (dy == -EXT)//topright
    {
      mask.at<uchar>(i-1,j) = 200;
      mask.at<uchar>(i,j+1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i-ptr,j+ptr)= 200;
      }
    }
    else if (dy == 0)//centerright
    {
      mask.at<uchar>(i+1,j+1) = 200;
      mask.at<uchar>(i-1,j+1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i,j+ptr)= 200;
      }
    }
    else if (dy == EXT)//bottomright
    {
      mask.at<uchar>(i+1,j) = 200;
      mask.at<uchar>(i,j+1) = 200;
      for (int ptr = 0; ptr <= EXT; ++ptr)
      {
          mask.at<uchar>(i+ptr,j+ptr)= 200;
      }
    }
  }            
}