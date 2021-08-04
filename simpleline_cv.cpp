
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/CommandHome.h>
#include <mavros_msgs/PositionTarget.h>
#include <sensor_msgs/Range.h>
#include <iostream>
#include <stdio.h>
//#include <wiringPi.h>
#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Byte.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"
#include <tf/transform_broadcaster.h>
#include <sys/time.h>
#include <pthread.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <math.h>

using namespace cv;
using namespace std;
//pthread_mutex_t mutex_cam_data;
pthread_mutex_t mutex_mptocv;
/// Global variables

double t = 0,fps,timecount=0;
char mpstate=0;
char template_mps=0;
std_msgs::Byte cvtomp;
int ttt=0,summ=0,ave=0,i=0,j=0,yxh=0,shu=0,midline=0,piancha=0;
float y_ctrl=0;
short int COL[640]={0},ROW[480]={0};
Mat frame,img,src,sec_2,gry_2;
Mat1b mask;

float constrain_float(float x,float bound)
{
	if( x > bound )
		return bound;
	else if( x < -bound )
		return -bound;
	else return x;
}

float jdz(float jj)
{
	if(jj<0) jj=jj*(-1);
	return jj;
}

void mptocv_callback(const std_msgs::Byte& msg)
{
	pthread_mutex_lock(&mutex_mptocv);
	template_mps=msg.data;
	pthread_mutex_unlock(&mutex_mptocv);
	ROS_INFO("video:%d" ,msg.data );
	//ROS_INFO("video:%.2f , %.2f", msg.pose.position.x, msg.pose.position.z);
}

int main(int argc,char **argv)
{
    ros::init(argc,argv,"star_detection");
    ros::NodeHandle node_obj;
	pthread_mutex_init (&mutex_mptocv, NULL);

    ros::Publisher tomain_publisher=node_obj.advertise<geometry_msgs::PoseStamped>("/mydrone", 10);//向飞控发送信息的话题???
    ros::Publisher number_publisher2=node_obj.advertise<std_msgs::Byte>("/cvtomp", 10);
    //ros::Publisher beeper_publisher=node_obj.advertise<std_msgs::Bool>("/drone/button", 10);
    ros::Subscriber number_subscriber1 = node_obj.subscribe("/my_change_mode",10,mptocv_callback);//高度的话题
    ros::Rate rate(30);
    geometry_msgs::PoseStamped msg_velocity;

     //打开一个默认的相机
    VideoCapture capture("/dev/v4l/by-id/usb-Generic_Rmoncam_A2_720P_200901010001-video-index0");
    printf("aa");
    //检查是否成功打开
    if (!capture.isOpened())
    {  printf("err");
        return -1;}

while(1)
{  
    ros::spinOnce();
    
    //under the mutex_lock traslate 
    pthread_mutex_lock(&mutex_mptocv);
    mpstate = template_mps;
    pthread_mutex_unlock(&mutex_mptocv);
	
    t = (double)cv::getTickCount();
    printf("bb");
    if(mpstate==1)
    {
	printf("cc");
	capture>>frame;
	Rect rec = Rect(160,120,320,240);
    src=frame(rec);
    cvtColor(src, gry_2, COLOR_BGR2GRAY);//变为灰度图
    blur(gry_2, gry_2, Size(7, 7));//均值滤波降噪threshold(gry_2, sec_2, 120, 255, THRESH_BINARY);
    threshold(gry_2, sec_2, 120, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(9,9)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
    morphologyEx(sec_2, sec_2, MORPH_DILATE, element);
    imshow("00", sec_2);
    for (i = 0; i < sec_2.cols; ++i)
    {
        for (j = 0; j < sec_2.rows; ++j)
        {
            if(sec_2.at<uchar>(j,i)==0)ttt++;
        }
        COL[i]=ttt;
        ttt=0;
               // printf("%d,,%d\n",i,COL[i]);
        summ+=COL[i];
    }
    ave=summ/sec_2.cols;
    summ=0;
    //printf("%d\n",ave);
    for (i = 0; i < sec_2.cols; ++i)
    {
        if(COL[i]>ave)
        {
            yxh+=i;
            shu++;
        }
    }
    if(yxh==0)
    {
        printf("nonono");
        midline=sec_2.cols/2;
    }
    else
    {
        midline=yxh/shu;
        yxh=0;
        shu=0;
    }
    piancha=(sec_2.cols/2)-midline;
    y_ctrl=constrain_float(piancha*0.0022,0.2);
    printf("%.2f\n",y_ctrl);
	msg_velocity.pose.position.x=0.2;
	msg_velocity.pose.position.y=y_ctrl;
	msg_velocity.pose.position.z=0;
	tomain_publisher.publish(msg_velocity);
    }
    waitKey(1);
    rate.sleep();
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    fps = 1.0 / t;
    timecount+=t;
    printf("FPS:%d//////miaoshu:%.4f\n",(int)fps,t);
}
  return(0);
}

