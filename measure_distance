
//28  48  100  96-103
//28  48  50   46-56
//52  48  ……

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define kuan 36  //need measured
#define kk 48.5   //need measured
#define k_juli kuan*kk

#define abs_m(a,b) ((a)>b)||((a)<(b*(-1)))
#define abs_l(a,b) ((a)<b)&&((a)>(b*(-1)))
#define pi acos(-1)
///if(abs_m(shu,15))
using namespace cv;
using namespace std;

Mat frame,src,gry_2,sec_2,can;
int count_lie=0,count_i=0;
int mid=0,ttt=0;
int wid=0,wid_last=0;


float jdz_f(float xl)
{
    if(xl<0)
        xl=xl*(-1);
    return xl;
}
int jdz_i(int xl)
{
    if(xl<0)
        xl=xl*(-1);
    return xl;
}

//geshu=lines.size();


////////////dbscan
//void dbscan_1d(float *jieju,int geshu,float *ave,int r)
//{
//    char mark[10]=0;
//    int geshu_remain;
//    geshu_remain=geshu;
//    float jieju_out;
//    mark[0]=1;
//    for(int i=0;i<geshu_remain;i++)
//    {
//        if(mark[i]==0)
//    }
//}

///////////change axis
void lineaxis_cv2zuoxia(Vec4i &old,int now[],int row)
{
    now[0]=old[0];
    now[1]=row-old[1];
    now[2]=old[2];
    now[3]=row-old[3];
}

////////////the width between two HoughLinesP(many--->two)
int width_line(vector<Vec4i> &dian,int geshu,int row)
{
    int dian_all[10][4];
    int jiedian[10],jiedian_ave[2],jieju=0,width_sum=0,width_ave=0,count_width=0;
    double jiao_sum=0,jiao_ave=0,jiao[10],xielv[10];
    //change_axis
    for(int i=0;i<geshu;i++)
    {
        lineaxis_cv2zuoxia(dian[i],dian_all[i],row);
    }

//    for(int i=0;i<geshu;i++)//////debug begin
//    {
//        for(int j=0;j<4;j++)
//            printf("%d ",dian[i][j]);
//        printf("\n");
//    }
//    printf("$$$$$$$$$$$$\n");
//    for(int i=0;i<geshu;i++)
//    {
//        for(int j=0;j<4;j++)
//            printf("%d ",dian_all[i][j]);
//        printf("\n");
//    }
//    ///////  debug end
    if(geshu>10) geshu=10;
    for(int i=0;i<geshu;i++)
    {
        if(dian_all[i][2]==dian_all[i][0])
        {
            jiao[i]=pi/2;
            jiao_sum+=jiao[i];
        }
        else
        {
            xielv[i]=(double)(dian_all[i][3]-dian_all[i][1])/(double)(dian_all[i][2]-dian_all[i][0]);
            jiao[i]=atan(xielv[i]);
            jiao_sum+=jiao[i];
        }
    }
    jiao_ave=jiao_sum/geshu;
//    for(int i=0;i<geshu;i++)///////debug
//    {
//        printf("%.4f&&",xielv[i]);
//    }
//    printf("\n");
//    printf("jiao_ave:%.5lf\n",jiao_ave);/////debug
    if(abs_l(jiao_ave,0.7854))
    {
        printf("rui\n");
        for(int i=0;i<geshu;i++)
        {
            if(dian_all[i][1]==dian_all[i][3])
                jiedian[i]=dian_all[i][1];
            else
                jiedian[i]=(int)((double)((dian_all[i][3]+dian_all[i][1])/2)-(double)((dian_all[i][2]+dian_all[i][0])/2)*tan(jiao_ave));
        }

//        for(int i=0;i<geshu;i++)///////debug
//        {
//            printf("%d^^",jiedian[i]);
//        }
//        printf("\n");
        //dbscan_1d(jiedian,geshu,jiedian_ave);
        //jieju=jdz_i(jiedian_ave[0]-jiedian_ave[1]);
        //width_ave=(int)(jieju*cos(jiao_ave));
        for(int i=0;i<(geshu-1);i++)
        {
            for(int j=(i+1);j<geshu;j++)
            {
                jieju=jdz_i(jiedian[i]-jiedian[j]);
                if(jieju>=4)
                {
                    width_sum+=ceil(jieju*cos(jiao_ave));
                    count_width++;
                }
//                printf("%d@@",jieju);
            }
        }
        if(count_width>0)
            width_ave=width_sum/count_width;
    }

    else
    {
        printf("dun\n");
        for(int i=0;i<geshu;i++)
        {
            if(dian_all[i][0]==dian_all[i][2])
                jiedian[i]=dian_all[i][0];
            else
                jiedian[i]=(int)((double)((dian_all[i][0]+dian_all[i][2])/2)-(double)((dian_all[i][1]+dian_all[i][3])/2)/tan(jiao_ave));
                //jiedian[i]=(int)(dian_all[i][2]-dian_all[i][3]/tan(jiao_ave));
        }
//        dbscan_1d(jiedian,geshu,jiedian_ave);
//        jieju=jdz_i(jiedian_ave[0]-jiedian_ave[1]);
//        width_ave=(int)(jieju*sin(jiao_ave));
        for(int i=0;i<(geshu-1);i++)
        {
            for(int j=(i+1);j<geshu;j++)
            {
                jieju=jdz_i(jiedian[i]-jiedian[j]);
                if(jieju>=4)
                {
                    width_sum+=(int)(jieju*sin(jiao_ave));
                    count_width++;
                }
            }
        }
        if(count_width>0)
        width_ave=width_sum/count_width;
    }
    return jdz_i(width_ave);
}



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
       Rect rec = Rect(160,120,320,240);
       src=frame(rec);
        cvtColor(src, gry_2, COLOR_BGR2GRAY);//变为灰度图
        blur(gry_2, gry_2, Size(3, 3));//均值滤波降噪
        threshold(gry_2, sec_2, 120, 255, THRESH_BINARY);

        Canny(sec_2,can,100,200,3,false);
        imshow("can",can);


        vector<Vec4i> lines;
            HoughLinesP(can, lines, 1, CV_PI/180, 80 ,80, 10 );
            for( size_t i = 0; i < lines.size(); i++ )
            {
                Vec4i l = lines[i];
                line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 3, LINE_AA);
                //printf("%d ,");
            }
            printf("\n");

            int geshu=lines.size();
            if(geshu!=0)
            {
                wid=width_line(lines,geshu,can.rows);
                if(wid==0)

                    wid=wid_last;
                wid_last=wid;
                printf("(((((((%d+++++++++\n",wid);

                float distance=(k_juli)/wid;
                printf("distance:::::%.4f\n",distance);
            }




//       for (int i = 0; i < sec_2.cols; ++i)
//         {
//                 for (int j = 0; j < sec_2.rows; ++j)
//                 {
//                    if(sec_2.at<uchar>(j,i)==0)ttt++;
//                 }
//                 if(ttt>20)
//                 {
//                    count_lie++;
//                    count_i+=i;
//                    ttt=0;
//                 }
//         }

//       if(count_lie!=0)
//       {
//           mid=count_i/count_lie;
//           char tt2[5]="";
//           sprintf(tt2, "%d", count_lie);
//           putText(src,tt2,Point(50,60),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),4,8);
//       }
//       count_i=0;
//       count_lie=0;
       imshow("nnn",src);
       imshow("binary",sec_2);
       //imwrite(str, frame);
       waitKey(1);
    }
       return 0;

}
