#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace cv;
using namespace std;

Mat frame;

int main()
{

    //打开一个默认的相机
    VideoCapture capture(0);
    printf("aa\n");
    //检查是否成功打开
    if (!capture.isOpened())
    {  printf("err");
        return -1;}

    while (1)
    {
       capture >> frame;


       int redp=2,step=13;
       char tt2[10]="",tt4[10]="";
       string str;
       string str0="/home/zhx02/qtceshi003/",str1="POINT:",str2,str3=";LINE:",str4,str5="(from_the_TOP,CCW).jpg";
       sprintf(tt2, "%d", redp);
       sprintf(tt4, "%d", step+1);
       str2=tt2;
       str4=tt4;
       str=str0+str1+str2+str3+str4+str5;
       cout<<str<<endl;
       imshow("nnn",frame);
       //imwrite(str, frame);
       waitKey(1);
    }
       return 0;

}
