#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

#include <algorithm>
#include <cstdint>


ros::ServiceClient client;


void drive_robot(float linear_x, float angular_z)
{
    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = linear_x;
    srv.request.angular_z = angular_z;

    if (client.call(srv))
    {
        ROS_INFO_STREAM_THROTTLE(
            1.0,
            "Drive service response: "
            << srv.response.msg_feedback
        );
    }
    else
    {
        ROS_ERROR_THROTTLE(
            1.0,
            "Failed to call /ball_chaser/command_robot"
        );
    }
}


void process_image_callback(const sensor_msgs::Image::ConstPtr& img)
{
    
    if (img->width == 0 ||
        img->height == 0 ||
        img->data.empty() ||
        img->step == 0)
    {
        ROS_WARN_THROTTLE(1.0, "Received an empty image");
        drive_robot(0.0, 0.0);
        return;
    }

    
    const unsigned int channels = 3;

    
    const uint8_t white_threshold = 250;

    bool ball_found = false;

    
    uint64_t white_x_sum = 0;
    uint64_t white_pixel_count = 0;

   
    const unsigned int sampling_step = 2;

    for (unsigned int row = 0;
         row < img->height;
         row += sampling_step)
    {
        for (unsigned int col = 0;
             col < img->width;
             col += sampling_step)
        {
            const size_t index =
                static_cast<size_t>(row) * img->step +
                static_cast<size_t>(col) * channels;

            if (index + 2 >= img->data.size())
            {
                continue;
            }

            const uint8_t red = img->data[index];
            const uint8_t green = img->data[index + 1];
            const uint8_t blue = img->data[index + 2];

            
            if (red >= white_threshold &&
                green >= white_threshold &&
                blue >= white_threshold)
            {
                ball_found = true;
                white_x_sum += col;
                ++white_pixel_count;
            }
        }
    }

    
    if (!ball_found || white_pixel_count == 0)
    {
        ROS_INFO_THROTTLE(1.0, "White ball not detected: stopping");
        drive_robot(0.0, 0.0);
        return;
    }

    
    const double ball_center_x =
        static_cast<double>(white_x_sum) /
        static_cast<double>(white_pixel_count);

    const double left_boundary =
        static_cast<double>(img->width) / 3.0;

    const double right_boundary =
        2.0 * static_cast<double>(img->width) / 3.0;

    if (ball_center_x < left_boundary)
    {
        
        ROS_INFO_THROTTLE(1.0, "Ball detected on the LEFT");
        drive_robot(0.0, 0.5);
    }
    else if (ball_center_x > right_boundary)
    {
        
        ROS_INFO_THROTTLE(1.0, "Ball detected on the RIGHT");
        drive_robot(0.0, -0.5);
    }
    else
    {
        
        ROS_INFO_THROTTLE(1.0, "Ball detected in the CENTER");
        drive_robot(0.5, 0.0);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");

    ros::NodeHandle n;

    client =
        n.serviceClient<ball_chaser::DriveToTarget>(
            "/ball_chaser/command_robot"
        );

    ROS_INFO("Waiting for command_robot service...");

    client.waitForExistence();

    ROS_INFO("command_robot service available");

    ros::Subscriber camera_subscriber =
        n.subscribe(
            "/camera/rgb/image_raw",
            10,
            process_image_callback
        );

    ROS_INFO(
        "process_image is subscribed to "
        "/camera/rgb/image_raw"
    );

    ros::spin();

    return 0;
}
