#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

#include <sstream>


ros::Publisher motor_command_publisher;

bool handle_drive_request(
    ball_chaser::DriveToTarget::Request& req,
    ball_chaser::DriveToTarget::Response& res)
{
    ROS_INFO(
        "DriveToTarget request: linear_x=%1.2f, angular_z=%1.2f",
        req.linear_x,
        req.angular_z
    );

    geometry_msgs::Twist motor_command;

    // Movimento lineare
    motor_command.linear.x = req.linear_x;
    motor_command.linear.y = 0.0;
    motor_command.linear.z = 0.0;

    
    motor_command.angular.x = 0.0;
    motor_command.angular.y = 0.0;
    motor_command.angular.z = req.angular_z;

    
    motor_command_publisher.publish(motor_command);

    std::stringstream feedback;
    feedback
        << "Requested linear velocity "
        << req.linear_x
        << " and angular velocity "
        << req.angular_z;

    res.msg_feedback = feedback.str();

    ROS_INFO_STREAM(res.msg_feedback);

    return true;
}

int main(int argc, char** argv)
{
    
    ros::init(argc, argv, "drive_bot");

    ros::NodeHandle n;

    
    motor_command_publisher =
        n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    
    ros::ServiceServer service =
        n.advertiseService(
            "/ball_chaser/command_robot",
            handle_drive_request
        );

    ROS_INFO("Drive bot service is ready.");

    // callback
    ros::spin();

    return 0;
}
