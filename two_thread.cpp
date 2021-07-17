#include <iostream>
#include <thread>
#include <unistd.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
  
using namespace std;
using namespace cv;
  
pthread_mutex_t mutex;
Mat frame0,frame1;
  
int totalNum = 1;
  
void thread01()
{
    pthread_mutex_lock(&mutex);
    VideoCapture capture0(0);
    if (!capture0.isOpened())
    {  printf("err_a");
        //return -1;
        }
    else
       {printf("aa");}
       pthread_mutex_unlock(&mutex);
       
    while(1)
    {
        capture0>>frame0;
        imshow("frame0",frame0);
        
        //pthread_mutex_lock(&mutex);
        //cout <<"aaaaaa"<< totalNum << endl;
        //pthread_mutex_unlock(&mutex);
        //usleep(3000);
        waitKey(1);
    }
}
void thread02()
{
    pthread_mutex_lock(&mutex);
    VideoCapture capture1(2);
    if (!capture1.isOpened())
    {  printf("err_b");
        //return -1;
        }
    else
       {printf("bb");}
       pthread_mutex_unlock(&mutex);
    while(1)
    {
        capture1>>frame1;
        imshow("frame1",frame1);
        
        //pthread_mutex_lock(&mutex);
        //cout <<"bbbbbb"<< totalNum << endl;
        //pthread_mutex_unlock(&mutex);
        //usleep(3000);
        waitKey(1);
    }   
}
  
int main()
{
    thread task01(thread01);
    thread task02(thread02);
    task01.detach();
    task02.detach();
    
    while(1)
    {
        
        pthread_mutex_lock(&mutex);
        if(totalNum>=20) totalNum=0;
        totalNum++;
        cout <<"mainmainmain"<< totalNum << endl;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return(0);
}
