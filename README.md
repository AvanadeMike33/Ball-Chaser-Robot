# Ball-Chaser-Robot

Ball Chaser Robot - README
Project Overview

This project implements a simple autonomous robot in ROS and Gazebo. The robot uses a camera to detect a white ball and automatically moves toward it.

The system consists of:

A differential drive robot
A camera sensor
A white target ball
An image processing node
A robot driving node
Gazebo simulation

When the white ball appears in the camera image:

Ball on the left → robot turns left
Ball on the right → robot turns right
Ball in the center → robot moves forward
Ball not detected → robot stops
Workspace Structure
catkin_ws
├── src
│   ├── my_robot
│   │   ├── launch
│   │   ├── urdf
│   │   ├── worlds
│   │   └── meshes
│   │
│   └── ball_chaser
│       ├── src
│       ├── launch
│       ├── srv
│       └── CMakeLists.txt
│
└── build

Requirements
Ubuntu
Ubuntu 20.04

ROS
ROS Noetic

Gazebo
Gazebo 11

Build Instructions

Open a terminal and move to the workspace:

cd ~/catkin_ws


Build the workspace:

catkin_make


Source the workspace:

source devel/setup.bash


To avoid repeating this every time:

echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc
source ~/.bashrc

Launching the Simulation
Terminal 1

Start ROS Master:

roscore

Terminal 2

Launch Gazebo world:

cd ~/catkin_ws
source devel/setup.bash

roslaunch my_robot world.launch


Wait until Gazebo is completely loaded.

Terminal 3

Spawn the robot:

cd ~/catkin_ws
source devel/setup.bash

roslaunch my_robot spawn.launch


The robot should appear inside Gazebo.

Terminal 4

Start Ball Chaser:

cd ~/catkin_ws
source devel/setup.bash

roslaunch ball_chaser ball_chaser.launch

Testing Robot Movement

Test manual movement:

rostopic pub /cmd_vel geometry_msgs/Twist -r 10 "
linear:
  x: 0.5
  y: 0.0
  z: 0.0
angular:
  x: 0.0
  y: 0.0
  z: 0.0"


The robot should move forward.

Stop robot:

rostopic pub /cmd_vel geometry_msgs/Twist -1 "
linear:
  x: 0.0
  y: 0.0
  z: 0.0
angular:
  x: 0.0
  y: 0.0
  z: 0.0"

White Ball Setup

Insert a sphere in Gazebo:

Insert → Sphere


Move the sphere in front of the robot.

The sphere must be:

WHITE


Recommended material:

Gazebo/White


If available:

Ambient  = 1 1 1 1
Diffuse  = 1 1 1 1
Specular = 1 1 1 1


Save the world:

File → Save World As...


Example:

~/catkin_ws/src/my_robot/worlds/ball_chaser.world

Updating the Robot

If the URDF/Xacro model is modified:

Delete current robot:

rosservice call /gazebo/delete_model '{model_name: my_robot}'


Respawn robot:

roslaunch my_robot spawn.launch


No need to restart Gazebo.

Rebuilding After Code Changes

Whenever you change:

drive_bot.cpp
process_image.cpp


rebuild:

cd ~/catkin_ws

catkin_make

source devel/setup.bash


Then relaunch:

roslaunch ball_chaser ball_chaser.launch

Useful ROS Commands

List active topics:

rostopic list


Check camera images:

rqt_image_view


Monitor robot commands:

rostopic echo /cmd_vel


Check node list:

rosnode list


Check active services:

rosservice list

Expected Behaviour
Ball Position	Robot ActionLeft Side	Turn Left
Center	Move Forward
Right Side	Turn Right
Not Detected	Stop
Success Criteria

The project is successful when:

Gazebo launches correctly.
The robot spawns successfully.
The camera publishes images.
The white ball is visible.
The image processing node detects the ball.
The drive node publishes movement commands.
The robot autonomously follows the white ball.
Shutdown

Close Ball Chaser:

Ctrl + C


Close Gazebo:

Ctrl + C


Close ROS Master:

Ctrl + C
