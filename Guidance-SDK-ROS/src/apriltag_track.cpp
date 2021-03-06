//#pragma once
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include<geometry_msgs/Vector3.h>
#include<cv_bridge/cv_bridge.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include<ros/ros.h>
#include <geometry_msgs/TransformStamped.h> //IMU
#include <geometry_msgs/Vector3Stamped.h> //velocity
#include <sensor_msgs/LaserScan.h> //obstacle distance && ultrasonic
#include<ros/publisher.h>
#include<dji_sdk/include/dji_sdk/dji_drone.h>
#include<dji_sdk/LocalPosition.h>
#include<Guidance-SDK-ROS/include/apriltag_dectect.h>
ros::Subscriber left_image_subr;
ros::Subscriber right_image_subr;
ros::Publisher   rel_dist_pub;
ros::Subscriber local_position_subscriber;
//ros::Subscribeglobalr depth_image_sub;
//ros::Subscriber imu_sub;
//ros::Subscriber velocity_sub;
//ros::Subscriber obstacle_distance_sub;
//ros::Subscriber ultrasonic_sub;
//ros::Subscriber position_sub;





const char* windowName = "apriltags_demo";

cv:: Mat left_;
cv:: Mat left_gray;
cv:: Mat right_;
float april_x;
float april_y;
float april_z;
double april_yaw;
//double april_yaw;
//double april_pitch;



/* left greyscale image */
void left_image_callbackr(const sensor_msgs::ImageConstPtr& left_img)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(left_img, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    cout<<"left "<<cv_ptr->image.size()<<endl;
    left_ = cv_ptr->image;

//    cv::imshow("left_image", cv_ptr->image);
    cv::waitKey(1);
}

/* right greyscale image */
void right_image_callbackr(const sensor_msgs::ImageConstPtr& right_img)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(right_img, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    right_ = cv_ptr->image;
//    cv::imshow("right_image", cv_ptr->image);
    cv::waitKey(1);
}

/* depth greyscale image */
void depth_image_callbackr(const sensor_msgs::ImageConstPtr& depth_img)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(depth_img, sensor_msgs::image_encodings::MONO16);
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv::Mat depth8(HEIGHT, WIDTH, CV_8UC1);
    cv_ptr->image.convertTo(depth8, CV_8UC1);
    cv::imshow("depth_image", depth8);
    cv::waitKey(1);
}
/* imu */
void imu_callback(const geometry_msgs::TransformStamped& g_imu)
{
    printf( "frame_id: %s stamp: %d\n", g_imu.header.frame_id.c_str(), g_imu.header.stamp.sec );
    printf( "imu: [%f %f %f %f %f %f %f]\n", g_imu.transform.translation.x, g_imu.transform.translation.y, g_imu.transform.translation.z,
                                                g_imu.transform.rotation.x, g_imu.transform.rotation.y, g_imu.transform.rotation.z, g_imu.transform.rotation.w );
}

/* velocity */
void velocity_callback(const geometry_msgs::Vector3Stamped& g_vo)
{
    printf( "frame_id: %s stamp: %d\n", g_vo.header.frame_id.c_str(), g_vo.header.stamp.sec );
    printf( "velocity: [%f %f %f]\n", g_vo.vector.x, g_vo.vector.y, g_vo.vector.z );
}

/* obstacle distance */
void obstacle_distance_callback(const sensor_msgs::LaserScan& g_oa)
{
    printf( "frame_id: %s stamp: %d\n", g_oa.header.frame_id.c_str(), g_oa.header.stamp.sec );
    printf( "obstacle distance: [%f %f %f %f %f]\n", g_oa.ranges[0], g_oa.ranges[1], g_oa.ranges[2], g_oa.ranges[3], g_oa.ranges[4] );
}

/* ultrasonic */
void ultrasonic_callback(const sensor_msgs::LaserScan& g_ul)
{
    printf( "frame_id: %s stamp: %d\n", g_ul.header.frame_id.c_str(), g_ul.header.stamp.sec );
    for (int i = 0; i < 5; i++)
        printf( "ultrasonic distance: [%f]  reliability: [%d]\n", g_ul.ranges[i], (int)g_ul.intensities[i] );
}

/* motion */
void position_callback(const geometry_msgs::Vector3Stamped& g_pos)
{
        printf("frame_id: %s stamp: %d\n", g_pos.header.frame_id.c_str(), g_pos.header.stamp.sec);
        for (int i = 0; i < 5; i++)
                printf("global position: [%f %f %f]\n", g_pos.vector.x, g_pos.vector.y, g_pos.vector.z);
}


// utility function to provide current system time (used below in
// determining frame rate at which images are being processed)
double tic() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return ((double)t.tv_sec + ((double)t.tv_usec)/1000000.);
}


#include <cmath>

#ifndef PI
const double PI = 3.14159265358979323846;
#endif
const double TWOPI = 2.0*PI;

/**
 * Normalize angle to be within the interval [-pi,pi].
 */
inline double standardRad(double t) {
  if (t >= 0.) {
    t = fmod(t+PI, TWOPI) - PI;
  } else {
    t = fmod(t-PI, -TWOPI) + PI;
  }
  return t;
}

/**
 * Convert rotation matrix to Euler angles
 */
void wRo_to_euler(const Eigen::Matrix3d& wRo, double& yaw, double& pitch, double& roll) {
    yaw = standardRad(atan2(wRo(1,0), wRo(0,0)));
    double c = cos(yaw);
    double s = sin(yaw);
    pitch = standardRad(atan2(-wRo(2,0), wRo(0,0)*c + wRo(1,0)*s));
    roll  = standardRad(atan2(wRo(0,2)*s - wRo(1,2)*c, -wRo(0,1)*s + wRo(1,1)*c));
}





void  left_image_callback(const sensor_msgs::ImageConstPtr& left_img)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(left_img, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    cout<<"left "<<cv_ptr->image.size()<<endl;
    left_ = cv_ptr->image;
 //   Demo::processImage(left_,left_);
//    cv::imshow("left_image", cv_ptr->image);
    cv::waitKey(1);
}
// here is where everything begins
int main(int argc, char* argv[])
{
    ros::init(argc, argv, "apriltag_track");
    ros::NodeHandle node_track;
    Demo demo;
    demo.setup();
    geometry_msgs::Vector3 april_dist;
    DJIDrone* drone = new DJIDrone(node_track);

    left_image_subr = node_track.subscribe("/guidance/left_image",  10, left_image_callback);
    right_image_subr = node_track.subscribe("/guidance/right_image", 10, right_image_callbackr);
    rel_dist_pub = node_track.advertise<geometry_msgs::Vector3>("apriltag_rel_dist",10);                 // computed relative distance by ApriTag_Track,published to drone_task node
    drone->local_position_subscriber = node_track.subscribe("dji_sdk/LocalPosition",10,local_position_subscriber_callback);
   // drone->local_position_subscriber =node_track.subscribe("dji_sdk/LocalPosition",10,drone->local_position_subscriber_callback);
//    depth_image_sub       = my_node.subscribe("/guidance/depth_image", 10, depth_image_callback);
//    imu_sub               = my_node.subscribe("/guidance/imu", 1, imu_callback);
//    velocity_sub          = my_node.subscribe("/guidance/velocity", 1, velocity_callback);
//    obstacle_distance_sub = my_node.subscribe("/guidance/obstacle_distance", 1, obstacle_distance_callback);
//	   ultrasonic_sub = my_node.subscribe("/guidance/ultrasonic", 1, ultrasonic_callback);
//	   position_sub = my_node.subscribe("/guidance/position", 1, position_callback);
    cout << "Processing Left image" << endl;
    while (ros::ok())
       {
       cout<< left_image_subr.getTopic()<<endl;
        ros::spinOnce();
        if(left_.size().width!=0)
            demo.processImage(left_, left_);
            april_dist.x = april_x;
            april_dist.y = april_y;
            april_dist.z = april_z;
            rel_dist_pub.publish(april_dist);
            if(!drone->local_position){
           float local_x = drone->local_position.x;
           float local_y = drone->local_position.y;
           float local_z = drone->local_position.z;
            }
           drone->local_position_navigation_send_request(local_x+april_dist.x,local_y+april_dist.y,local_z+april_dist.z);
       }
    return 0;

}























