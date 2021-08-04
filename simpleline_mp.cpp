

#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/CommandLong.h>
#include <mavros_msgs/CommandHome.h>
#include <mavros_msgs/PositionTarget.h>
#include <sensor_msgs/Range.h>
#include <iostream>
#include <stdio.h>
//#include <wiringPi.h>
#include "opencv2/opencv.hpp"
#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Byte.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"
#include <tf/transform_broadcaster.h>
#include   <sys/time.h>
#include <pthread.h>
#include <mavros_msgs/State.h>


pthread_mutex_t mutex_t265;


using namespace cv;
using namespace std;

float yaw_old = 0;
float posz_t265 = 0;

float posx_t265 = 0;
float posy_t265 = 0;

float posz_t265_local = 0;
float posx_t265_start = 0;
float posy_t265_start = 0;
float yaw_t265_start = 0;
signed char state = -1;
int temp_step = 0;

bool t265_ready = false;



float constrain_float(float x, float bound)
{
	if (x > bound)
		return bound;
	else if (x < -bound)
		return -bound;
	else return x;
}

//���ظ߶ȵ�����
float Height = 0;//�ɻ��߶�
float Height_init = 0;
int height_flag = 1;
pthread_mutex_t mutex_height;
void pose_callback(const geometry_msgs::PoseStamped& msg)
{
	if (height_flag == 0)
	{
		height_flag = 1;
		Height_init = msg.pose.position.z;
	}
	pthread_mutex_lock(&mutex_height);
	Height = msg.pose.position.z - Height_init;
	//posx_t265=msg.pose.position.y;
	//posy_t265=msg.pose.position.x;
	//x_distance=msg.pose.position.x;
	pthread_mutex_unlock(&mutex_height);
	//	ROS_INFO("Height  [%f]",Height);
}

//ͼ�񷵻ص�����
bool video_ready = false;
float position_x = 0;//ͼ�񷵻ص�����
float position_y = 50;
float position_z = 1;
pthread_mutex_t mutex_cam_data;
void drone_callback(const geometry_msgs::PoseStamped& msg) //opencv
{
	video_ready = true;
	pthread_mutex_lock(&mutex_cam_data);
	position_x = msg.pose.position.x;
	position_y = msg.pose.position.y;
	//position_z=msg.pose.position.z;
	pthread_mutex_unlock(&mutex_cam_data);
	//	ROS_INFO("video:%.2f , %.2f", msg.pose.position.x, msg.pose.position.z);
}

char cvstate = 0;
pthread_mutex_t mutex_cvtomp;
void cvtomp_callback(const std_msgs::Byte& msg)
{
	pthread_mutex_lock(&mutex_cvtomp);
	cvstate = msg.data;
	pthread_mutex_unlock(&mutex_cvtomp);
	//ROS_INFO("video:%.2f , %.2f", msg.pose.position.x, msg.pose.position.z);
}

void camera_callback(const nav_msgs::Odometry& msg)
{
	t265_ready = true;
	pthread_mutex_lock(&mutex_t265);
	posx_t265 = msg.pose.pose.position.x;
	posy_t265 = msg.pose.pose.position.y;
	posz_t265 = msg.pose.pose.position.z;//added by ujj
	//Height=msg.pose.pose.position.z-posz_t265_start;//ujj
	pthread_mutex_unlock(&mutex_t265);
	//ujj	ROS_INFO("video:%.2f , %.2f ,%.2f", msg.pose.position.x, msg.pose.position.y,msg.pose.position.z);
	//		ROS_INFO("video:%.2f , %.2f ,%.2f", posx_t265,posy_t265,posz_t265);
}

mavros_msgs::State current_state;
pthread_mutex_t mutex_State;
void state_cb(const mavros_msgs::State::ConstPtr& msg)
{
	ROS_INFO("*******HEART_BEAT*******");
	pthread_mutex_lock(&mutex_State);
	current_state = *msg;
	pthread_mutex_unlock(&mutex_State);
}

int sleeptime = 0;
int move_count = 0;
int consistant_count = 0;
unsigned int loop_timer = 0;

float take_off_Height = 0.5;

int main(int argc, char** argv)
{
	ros::init(argc, argv, "main_process");
	//	wiringPiSetup(); 	pinMode(0, OUTPUT);  digitalWrite(0, HIGH); //��ݮ��
	pthread_mutex_init(&mutex_height, NULL);
	pthread_mutex_init(&mutex_cam_data, NULL);
	pthread_mutex_init(&mutex_t265, NULL);
	pthread_mutex_init(&mutex_cvtomp, NULL);
	pthread_mutex_init(&mutex_State, NULL);

	ros::NodeHandle node_obj;
	ros::Publisher number_publisher1 = node_obj.advertise<std_msgs::Byte>("/my_change_mode", 10);//��ͼ������Ϣ�Ļ���
	ros::Subscriber number_subscriber2 = node_obj.subscribe("/mydrone", 10, drone_callback);//opencv
	ros::Subscriber number_subscriber4 = node_obj.subscribe("/cvtomp", 10, cvtomp_callback);

	ros::Subscriber number_subscriber3 = node_obj.subscribe("/camera/odom/sample", 10, camera_callback);//T265��������
	ros::Subscriber number_subscriber1 = node_obj.subscribe("/mavros/local_position/pose", 10, pose_callback);//�ɿط��� ��λ�� 
	ros::Subscriber state_sub = node_obj.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);//�ɿط��ص�״̬

	ros::spinOnce();

	ros::ServiceClient takeoff_client = node_obj.serviceClient<mavros_msgs::CommandTOL>("mavros/cmd/takeoff");//���
	ros::ServiceClient set_mode_client = node_obj.serviceClient<mavros_msgs::SetMode>("mavros/set_mode"); //����35ģʽ
	ros::Publisher number_publisher2 = node_obj.advertise<mavros_msgs::PositionTarget>("/mavros/setpoint_raw/local", 10);//��ɿط�������
	ros::ServiceClient comand_long_client = node_obj.serviceClient<mavros_msgs::CommandLong>("mavros/cmd/command");	 //CMD����
	ros::ServiceClient landing_client = node_obj.serviceClient<mavros_msgs::CommandTOL>("mavros/cmd/land"); //����

	ros::spinOnce();
	height_flag = 0;

	ros::Rate rate(200);
	std_msgs::Byte mptocv;
	mptocv.data = 1;

	ros::AsyncSpinner spinner(3);
	spinner.start();

	while (ros::ok())
	{

		if (++loop_timer >= 10)
		{
			pthread_mutex_lock(&mutex_cam_data);
			float goal_x_local = position_x;//opencv
			float goal_y_local = position_y;
			//float goal_z_local=position_z;
			pthread_mutex_unlock(&mutex_cam_data);

			pthread_mutex_lock(&mutex_height);
			float Height_local = Height;
			float posx_t265_local = (-1) * posx_t265;
			float posy_t265_local = (-1) * posy_t265;
			pthread_mutex_unlock(&mutex_height);

			pthread_mutex_lock(&mutex_t265);
			//��ǰ�ĽǶȡ���������
			float posz_t265_local = posz_t265;//added by ujj
			pthread_mutex_unlock(&mutex_t265);

			switch (state)
			{
			case -1:
			{
				ROS_INFO("video_ready  [%d]", video_ready);
				ROS_INFO("t265_ready  [%d]", t265_ready);

				//if(video_ready&&t265_ready)
				if (t265_ready)  //�Ƿ��õ���
				{
					state++;
					ROS_INFO("*******T265 is Ready*******");
				}
				else
					state = -1;
			}
			break;

			//�����ɿ�
			case 0:
			{
				if (++sleeptime >= 20 && temp_step == 0) //������
				{
					sleeptime = 0;
					mavros_msgs::CommandLong Reset_Sent_Cmd;
					Reset_Sent_Cmd.request.command = 0;
					Reset_Sent_Cmd.request.param1 = 0; //����������������
					if (comand_long_client.call(Reset_Sent_Cmd))
					{
						ROS_INFO("***Reset_CMD is sent***");
						temp_step++;
					}
				}
				else if (temp_step)//1��
				{
					//ѯ��һ��
					if (temp_step && current_state.connected)
					{
						temp_step = 0;
						ROS_INFO("***Reset Successfully***");
						state++;
						//state = 50 ;

					}
					else
					{
						ROS_INFO("***Waiting for reset ready***");

					}
				}
			}
			break;


			//����
			case 1:
			{
				if (++sleeptime >= 200) //10��
				{
					sleeptime = 0;
					mavros_msgs::SetMode SetMode_Cmd;

					if (set_mode_client.call(SetMode_Cmd))
					{
						state++;
						ROS_INFO("Set Mode");
					}
				}
				else
					state = 1;
				break;
			}
			


			case 2:
			{
				if (++sleeptime >= 40) //5��
				{
					sleeptime = 0;
					ROS_INFO("State 2");
					mavros_msgs::CommandTOL takeoff_cmd;
					takeoff_cmd.request.altitude = take_off_Height;//�߶�
					takeoff_cmd.request.longitude = 0;
					takeoff_cmd.request.latitude = 0;
					if (takeoff_client.call(takeoff_cmd))
					{
						state++;
						ROS_INFO("State 3");
						ROS_INFO("TAKEOFF responce [%d] ", takeoff_cmd.response.success);
						ROS_INFO("TAKEOFF responce [%hhu] ", takeoff_cmd.response.result);

					}
				}
				break;
			}
			


			case 3:
			{
				if (posz_t265_local > take_off_Height)
				{
					sleeptime = 0;
					state++;
					//state = 8;
					ROS_INFO("State 4");
					mptocv.data=1;
					number_publisher1.publish(mptocv);
				}
				else
					state = 3;
			}
			break;

			//ǰ�� �ٶ� 0.2  5��
			case 4:
			{
				if (++sleeptime >= 40)    //15sec
				{
					//sleeptime = 0;
					if(posx_t265_local<=2.2)
					{
						mavros_msgs::PositionTarget msg_velocity;
						msg_velocity.coordinate_frame = 12;
						msg_velocity.type_mask = 1 | 2 | 4 | 64 | 128 | 256 | 512 | 1024 | 2048;
						msg_velocity.velocity.x = goal_x_local;
						msg_velocity.velocity.y = goal_y_local;
						msg_velocity.velocity.z = 0;
						//msg_velocity.yaw=0;
						number_publisher2.publish(msg_velocity);
					}
					else
					{
						state++;
						mavros_msgs::PositionTarget msg_velocity;
						msg_velocity.coordinate_frame = 12;
						msg_velocity.type_mask = 1 | 2 | 4 | 64 | 128 | 256 | 512 | 1024 | 2048;
						msg_velocity.velocity.x = 0;
						msg_velocity.velocity.y = 0;
						msg_velocity.velocity.z = 0;
						//msg_velocity.yaw=0;
						number_publisher2.publish(msg_velocity);
						ROS_INFO("State 5");
					}
				}
				else
					state = 4;
			}
			break;

			case 5:
			{
				if (++sleeptime >= 90)
				{
					sleeptime = 0;
					mavros_msgs::CommandTOL land_cmd;
					land_cmd.request.altitude = 0;
					land_cmd.request.longitude = 0;
					land_cmd.request.latitude = 0;
					if (landing_client.call(land_cmd))
					{
						state = 50;
						ROS_INFO("State 50");
					}
				}
			}
			break;

			case 50:
			{
				if (++sleeptime >= 40)
				{
					sleeptime = 0;
					state = 50;
					ROS_INFO("State 50");

				}
			}
			break;
			}

			loop_timer = 0;
		}
		rate.sleep();
	}
	return 0;
}



