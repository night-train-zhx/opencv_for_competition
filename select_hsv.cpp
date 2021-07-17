#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include<math.h>
using namespace cv;
using namespace std;


const int h_max=255,s_max=255,v_max=255;
int h_l=25,s_l=41,v_l=106,h_h=61,s_h=101,v_h=219;
//red
//int h_l1=0,s_l1=70,v_l1=50,h_h1=10,s_h1=255,v_h1=255;
//int h_l2=0,s_l2=70,v_l2=50,h_h2=10,s_h2=255,v_h2=255;
Mat image,new_image,hsv;
Mat1b mask;

static void on_trackbar(int,void*)
{
    cvtColor(image, hsv, COLOR_BGR2HSV);//转为HSV
    Mat1b mask;
    inRange(hsv,Scalar(h_l,s_l,v_l),Scalar(h_h,s_h,v_h),mask);
//    red
//    Mat1b mask1,mask2;
//    inRange(hsv,Scalar(0,70,50),Scalar(10,255,255),mask1);
//    inRange(hsv,Scalar(170,70,50),Scalar(180,255,255),mask2);
//    mask = mask1|mask2;
      Mat element1 = getStructuringElement(MORPH_RECT, Size(2,2));
      morphologyEx(mask,mask, MORPH_ERODE, element1);
      Mat element2 = getStructuringElement(MORPH_RECT, Size(2,2));
      morphologyEx(mask,mask, MORPH_DILATE, element2);
      Mat element3 = getStructuringElement(MORPH_RECT, Size(2,2));
      morphologyEx(mask,mask, MORPH_ERODE, element3);
    imshow("mask",mask);
}

int main()
{
    image = imread("/home/zhx02/qtceshi/fs_1.jpg");
    resize(image, image, Size(), 0.3, 0.3, INTER_NEAREST);
    GaussianBlur(image, image, Size(3, 3), 0, 0);

    namedWindow("New Image", WINDOW_NORMAL);
    createTrackbar("h_l","New Image",&h_l,h_max,on_trackbar);
    createTrackbar("s_l","New Image",&s_l,s_max,on_trackbar);
    createTrackbar("v_l","New Image",&v_l,v_max,on_trackbar);
    createTrackbar("h_h","New Image",&h_h,h_max,on_trackbar);
    createTrackbar("s_h","New Image",&s_h,s_max,on_trackbar);
    createTrackbar("l_h","New Image",&v_h,v_max,on_trackbar);
    on_trackbar(h_l,0);
    waitKey(0);
    return 0;
}
