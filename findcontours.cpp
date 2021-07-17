
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace cv;

Mat hsv;
int main()
{
    Mat imageSource=imread("/home/zhx02/qtceshi/fs_1.jpg");
    resize(imageSource, imageSource, Size(), 0.3, 0.3, INTER_NEAREST);
//    imshow("Source Image",imageSource);
    Mat image,gry_2;
//    cvtColor(imageSource, gry_2, COLOR_BGR2GRAY);//变为灰度图
//    blur(gry_2, gry_2, Size(3, 3));//均值滤波降噪
//    threshold(gry_2, gry_2, 120, 255, THRESH_BINARY);

    int h_l=25,s_l=41,v_l=106,h_h=61,s_h=101,v_h=219;
    cvtColor(imageSource, hsv, COLOR_BGR2HSV);//转为HSV
    Mat1b mask;
    inRange(hsv,Scalar(h_l,s_l,v_l),Scalar(h_h,s_h,v_h),mask);
    Mat element1 = getStructuringElement(MORPH_RECT, Size(2,2));
    morphologyEx(mask,mask, MORPH_ERODE, element1);
    Mat element2 = getStructuringElement(MORPH_RECT, Size(2,2));
    morphologyEx(mask,mask, MORPH_DILATE, element2);
    Mat element3 = getStructuringElement(MORPH_RECT, Size(2,2));
    morphologyEx(mask,mask, MORPH_ERODE, element3);

    Canny(mask,image,100,200,3,false);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(image,contours,hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE,Point());
    vector<Rect> boundRect(contours.size());  //定义外接矩形集合
    vector<RotatedRect> box(contours.size()); //定义最小外接矩形集合
    Point2f rect[4];
        for(int i=0; i<contours.size(); i++)
        {
            if(contours[i].size()>100)//mianji
            {
                box[i] = minAreaRect(Mat(contours[i]));  //计算每个轮廓最小外接矩形
                boundRect[i] = boundingRect(Mat(contours[i]));
                circle(imageSource, Point(box[i].center.x, box[i].center.y), 5, Scalar(0, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
                box[i].points(rect);  //把最小外接矩形四个端点复制给rect数组
                rectangle(imageSource, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 2, 8);
                for(int j=0; j<4; j++)
                {
                    line(imageSource, rect[j], rect[(j+1)%4], Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
                }
            }
        }
        imshow("dst", imageSource);

    waitKey(0);
    return 0;
}

