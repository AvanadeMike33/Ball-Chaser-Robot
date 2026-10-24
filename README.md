# Ball Chaser Robot

A ROS and Gazebo simulation project in which a differential-drive robot uses its onboard camera to detect a white ball and autonomously move toward it.

## Table of Contents

- [Project Overview](#project-overview)
- [System Architecture](#system-architecture)
- [Expected Behavior](#expected-behavior)
- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Workspace Setup](#workspace-setup)
- [Build Instructions](#build-instructions)
- [Launching the Project](#launching-the-project)
- [Adding a Permanent White Ball](#adding-a-permanent-white-ball)
- [Testing the Camera](#testing-the-camera)
- [Testing Robot Motion](#testing-robot-motion)
- [Updating the Robot Without Restarting Gazebo](#updating-the-robot-without-restarting-gazebo)
- [Robot Stability](#robot-stability)
- [Development Workflow](#development-workflow)
- [Useful ROS Commands](#useful-ros-commands)
- [Troubleshooting](#troubleshooting)
- [Success Checklist](#success-checklist)
- [Shutdown](#shutdown)

## Project Overview

The Ball Chaser project contains two ROS packages:

- `my_robot`: defines the robot, sensors, Gazebo configuration, launch files, meshes, and simulation world.
- `ball_chaser`: contains the ROS nodes and service used to detect the white ball and command the robot.

The main software components are:

1. `process_image`: subscribes to the robot camera image, searches for white pixels, and determines the horizontal position of the ball.
2. `drive_bot`: provides a service that publishes linear and angular velocity commands to `/cmd_vel`.
3. Gazebo differential-drive plugin: receives `/cmd_vel` commands and applies them to the robot wheels.
4. Gazebo camera plugin: publishes images from the robot camera.

## System Architecture

```text
Gazebo camera
     |
     v
Camera image topic
     |
     v
process_image node
     |
     v
DriveToTarget service request
     |
     v
drive_bot node
     |
     v
/cmd_vel
     |
     v
Gazebo differential-drive controller
     |
     v
Robot motion
```

## Expected Behavior

| White ball position in the camera image | Robot action |
|---|---|
| Left side | Turn left |
| Center | Move forward |
| Right side | Turn right |
| Not visible | Stop |

## Requirements

This project is intended for a ROS 1 catkin workspace with Gazebo Classic.

Typical compatible environment:

- Ubuntu with ROS 1 installed
- ROS Noetic or the ROS distribution used by the course environment
- Gazebo Classic
- `catkin_make`
- C++ compiler
- ROS packages for Gazebo, camera messages, geometry messages, and ROS services

> Use the ROS distribution already configured in your environment. Package names and commands can vary between ROS distributions.

## Project Structure

```text
catkin_ws/
├── src/
│   ├── CMakeLists.txt
│   │
│   ├── my_robot/
│   │   ├── CMakeLists.txt
│   │   ├── package.xml
│   │   ├── launch/
│   │   │   ├── world.launch
│   │   │   └── spawn.launch
│   │   ├── meshes/
│   │   │   └── hokuyo.dae
│   │   ├── urdf/
│   │   │   ├── my_robot.xacro
│   │   │   └── my_robot.gazebo
│   │   ├── worlds/
│   │   │   └── ball_chaser.world
│   │   └── rviz/
│   │
│   └── ball_chaser/
│       ├── CMakeLists.txt
│       ├── package.xml
│       ├── launch/
│       │   └── ball_chaser.launch
│       ├── src/
│       │   ├── drive_bot.cpp
│       │   └── process_image.cpp
│       └── srv/
│           └── DriveToTarget.srv
│
├── build/
├── devel/
└── logs/
```

The `build`, `devel`, and `logs` directories are generated automatically and should not normally be created or edited manually.

## Workspace Setup

### Create the workspace

```bash
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace
```

### Create the package directories manually

Use this only if the project files do not already exist:

```bash
mkdir -p ~/catkin_ws/src/my_robot/launch
mkdir -p ~/catkin_ws/src/my_robot/meshes
mkdir -p ~/catkin_ws/src/my_robot/urdf
mkdir -p ~/catkin_ws/src/my_robot/worlds
mkdir -p ~/catkin_ws/src/my_robot/rviz

mkdir -p ~/catkin_ws/src/ball_chaser/launch
mkdir -p ~/catkin_ws/src/ball_chaser/src
mkdir -p ~/catkin_ws/src/ball_chaser/srv
```

### Display the directory tree

If `tree` is installed:

```bash
tree ~/catkin_ws/src
```

Otherwise:

```bash
find ~/catkin_ws/src -maxdepth 4 -print
```

## Build Instructions

Open a terminal and run:

```bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

If the build succeeds, make the workspace available automatically in new terminals:

```bash
echo 'source ~/catkin_ws/devel/setup.bash' >> ~/.bashrc
source ~/.bashrc
```

Verify that ROS can locate both packages:

```bash
rospack find my_robot
rospack find ball_chaser
```

## Launching the Project

The exact terminal sequence depends on your launch files. The following workflow supports a project where the world, robot spawn, and Ball Chaser nodes are launched separately.

### Terminal 1: Start ROS Master

```bash
roscore
```

Keep this terminal open.

### Terminal 2: Launch the Gazebo world

```bash
cd ~/catkin_ws
source devel/setup.bash
roslaunch my_robot world.launch
```

Wait until Gazebo has fully loaded.

### Terminal 3: Spawn the robot

```bash
cd ~/catkin_ws
source devel/setup.bash
roslaunch my_robot spawn.launch
```

If `world.launch` already spawns the robot, do not launch `spawn.launch` a second time.

### Terminal 4: Start the Ball Chaser nodes

```bash
cd ~/catkin_ws
source devel/setup.bash
roslaunch ball_chaser ball_chaser.launch
```

### Verify active nodes

```bash
rosnode list
```

Expected nodes normally include the Gazebo nodes and the Ball Chaser nodes.

### Verify active topics

```bash
rostopic list
```

Confirm that a camera image topic and `/cmd_vel` are available.

## Adding a Permanent White Ball

The ball must be white because the image-processing node detects white pixels in the camera image.

### Add the ball from the Gazebo UI

1. Open Gazebo.
2. Open the **Insert** panel.
3. Insert a sphere.
4. Move the sphere in front of the robot camera.
5. Select the sphere and open its visual or material properties.
6. Choose `Gazebo/White` if it is available.
7. If individual color channels are available, use:

```text
Ambient:  1 1 1 1
Diffuse:  1 1 1 1
Specular: 1 1 1 1
```

8. Save the complete world using **File > Save World As...**.
9. Save it in the package world directory, for example:

```text
~/catkin_ws/src/my_robot/worlds/ball_chaser.world
```

Adding the sphere without saving the world only keeps it for the current Gazebo session.

### Permanent SDF model for the ball

If the Gazebo UI does not preserve the white material correctly, add the following model inside the `<world>` element of the `.world` file:

```xml
<model name="white_ball">
  <pose>2 0 0.25 0 0 0</pose>

  <link name="ball_link">
    <inertial>
      <mass>0.5</mass>
      <inertia>
        <ixx>0.005</ixx>
        <iyy>0.005</iyy>
        <izz>0.005</izz>
        <ixy>0</ixy>
        <ixz>0</ixz>
        <iyz>0</iyz>
      </inertia>
    </inertial>

    <collision name="ball_collision">
      <geometry>
        <sphere>
          <radius>0.25</radius>
        </sphere>
      </geometry>
    </collision>

    <visual name="ball_visual">
      <geometry>
        <sphere>
          <radius>0.25</radius>
        </sphere>
      </geometry>
      <material>
        <ambient>1 1 1 1</ambient>
        <diffuse>1 1 1 1</diffuse>
        <specular>1 1 1 1</specular>
      </material>
    </visual>
  </link>
</model>
```

The example pose places the ball two meters in front of the world origin. Adjust the pose to match the robot's starting position and camera direction.

## Testing the Camera

Install or launch an image viewer available in your ROS environment:

```bash
rqt_image_view
```

In the image viewer:

1. Select the robot camera image topic.
2. Confirm that the image updates.
3. Move the white ball in front of the camera.
4. Confirm that the ball is clearly visible and sufficiently bright.

Find camera-related topics with:

```bash
rostopic list | grep -E 'image|camera'
```

Inspect the publication rate by replacing the topic below with the actual camera topic:

```bash
rostopic hz /camera/rgb/image_raw
```

Inspect camera message information:

```bash
rostopic info /camera/rgb/image_raw
```

## Testing Robot Motion

### Move forward

```bash
rostopic pub /cmd_vel geometry_msgs/Twist -r 10 \
'{linear: {x: 0.5, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}'
```

Press `Ctrl+C` to stop publishing.

### Turn left

```bash
rostopic pub /cmd_vel geometry_msgs/Twist -r 10 \
'{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.5}}'
```

### Turn right

```bash
rostopic pub /cmd_vel geometry_msgs/Twist -r 10 \
'{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: -0.5}}'
```

### Send one stop command

```bash
rostopic pub /cmd_vel geometry_msgs/Twist -1 \
'{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}'
```

### Monitor autonomous movement commands

```bash
rostopic echo /cmd_vel
```

## Updating the Robot Without Restarting Gazebo

When only the Xacro, URDF, visual geometry, collision geometry, mass, or joint configuration changes, Gazebo can remain open.

### Delete the current robot

```bash
rosservice call /gazebo/delete_model '{model_name: my_robot}'
```

The model name must match the name used when spawning the robot. Check current Gazebo models with:

```bash
rosservice call /gazebo/get_world_properties
```

### Spawn the updated robot

```bash
cd ~/catkin_ws
source devel/setup.bash
roslaunch my_robot spawn.launch
```

If the spawn launch file uses the Xacro file, the updated robot description will be processed when the robot is spawned again.

## Robot Stability

Moving the robot upright with Gazebo Rotate Mode changes only the current simulation state. It does not permanently correct the URDF/Xacro model.

For permanent stability:

- Remove unnecessary tall components such as the antenna.
- Keep the center of mass low.
- Keep the chassis inertial origin realistic.
- Ensure the wheels and caster contacts reach the ground correctly.
- Avoid extremely small or unrealistic masses and inertias.
- Keep the left and right wheel joints symmetric.
- Verify that wheel axes match the wheel geometry orientation.
- Configure friction and contact parameters in `my_robot.gazebo`.
- Spawn the robot slightly above the ground so it settles naturally without intersecting the floor.

After removing the antenna, delete and respawn the robot to test the saved model.

### Check the generated URDF

Generate URDF from Xacro:

```bash
rosrun xacro xacro ~/catkin_ws/src/my_robot/urdf/my_robot.xacro \
  > /tmp/my_robot.urdf
```

Validate the robot description:

```bash
check_urdf /tmp/my_robot.urdf
```

Generate a link-joint graph if the required tools are installed:

```bash
urdf_to_graphiz /tmp/my_robot.urdf
```

## Development Workflow

### After changing C++ code

For changes to `drive_bot.cpp`, `process_image.cpp`, service definitions, `CMakeLists.txt`, or `package.xml`:

```bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

Stop and relaunch the affected nodes:

```bash
roslaunch ball_chaser ball_chaser.launch
```

### After changing Xacro or Gazebo robot configuration

1. Save the file.
2. Delete the model from Gazebo.
3. Spawn it again.
4. Rebuild only if the package build configuration or compiled plugin code changed.

### After changing the world

1. Save the `.world` file.
2. Confirm that `world.launch` loads that file.
3. Restart the Gazebo world if the saved world itself must be reloaded.

## Useful ROS Commands

### Packages

```bash
rospack find my_robot
rospack find ball_chaser
```

### Nodes

```bash
rosnode list
rosnode info /process_image
rosnode info /drive_bot
```

Use the actual node names shown by `rosnode list`.

### Topics

```bash
rostopic list
rostopic echo /cmd_vel
rostopic info /cmd_vel
```

### Services

```bash
rosservice list
rosservice type /ball_chaser/command_robot
rosservice info /ball_chaser/command_robot
```

Replace the service name if your project uses a different namespace.

### Parameters

```bash
rosparam list
rosparam get /robot_description
```

### Gazebo services

```bash
rosservice list | grep gazebo
rosservice call /gazebo/pause_physics
rosservice call /gazebo/unpause_physics
rosservice call /gazebo/reset_simulation
rosservice call /gazebo/reset_world
```

## Troubleshooting

### `roslaunch` cannot find a package

Symptoms:

```text
Resource not found: my_robot
```

or:

```text
Resource not found: ball_chaser
```

Fix:

```bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
rospack profile
```

Then verify:

```bash
rospack find my_robot
rospack find ball_chaser
```

### `catkin_make` reports no target or no Makefile

Run `catkin_make` from the workspace root, not from `src` or a package directory:

```bash
cd ~/catkin_ws
catkin_make
```

### Gazebo opens but the robot is missing

Check whether the robot was spawned:

```bash
rosservice call /gazebo/get_world_properties
```

Then relaunch:

```bash
roslaunch my_robot spawn.launch
```

Review the terminal for Xacro, URDF, mesh, or package-path errors.

### The model already exists

If Gazebo reports that `my_robot` already exists:

```bash
rosservice call /gazebo/delete_model '{model_name: my_robot}'
roslaunch my_robot spawn.launch
```

### The robot falls over

1. Remove the antenna from the Xacro file.
2. Save the file.
3. Delete and respawn the robot.
4. Verify wheel and caster contact with the floor.
5. Confirm realistic masses and inertias.
6. Check wheel friction in `my_robot.gazebo`.
7. Spawn the robot with a neutral roll, pitch, and yaw.

Do not rely on Gazebo Rotate Mode as a permanent correction.

### The robot does not move

Check `/cmd_vel`:

```bash
rostopic echo /cmd_vel
```

If commands are published but the robot does not move:

- Verify that the Gazebo differential-drive plugin is loaded.
- Verify that the plugin uses the correct left and right wheel joint names.
- Verify that the plugin command topic is `/cmd_vel`.
- Check Gazebo and terminal logs for plugin errors.
- Confirm that physics is not paused.

Resume physics with:

```bash
rosservice call /gazebo/unpause_physics
```

### The robot moves in the wrong direction

Check:

- Wheel joint axes.
- Left and right joint assignment in the differential-drive plugin.
- Positive linear and angular command signs.
- Wheel geometry orientation.

### The camera image is unavailable

Check available topics:

```bash
rostopic list | grep -E 'image|camera'
```

Check the camera plugin in `my_robot.gazebo`, including the topic name and reference link.

### The white ball is not detected

- Confirm that the ball is visible in `rqt_image_view`.
- Use a pure white material.
- Place the ball inside the camera field of view.
- Ensure adequate world lighting.
- Move the ball closer to the camera.
- Confirm that `process_image` subscribes to the actual camera topic.
- Monitor the Ball Chaser terminal for callback or service messages.

### The ball appears gray

A white material may still look gray because of scene lighting and shading. Set both ambient and diffuse channels to white and verify the camera image rather than relying only on the main Gazebo viewport.

### Service call fails

List the services:

```bash
rosservice list
```

Inspect the target service:

```bash
rosservice type /ball_chaser/command_robot
rossrv show ball_chaser/DriveToTarget
```

Confirm that `drive_bot` is running and that the service package was rebuilt after changes to `DriveToTarget.srv`.

### C++ changes do not appear

Rebuild and source the workspace:

```bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

Then stop and relaunch the modified node.

## Shutdown

Stop each launch process with:

```bash
Ctrl+C
```

If Gazebo remains open after the launch process stops:

```bash
killall gzserver gzclient
```

Use `killall` only when Gazebo has not closed normally.

## Notes

- Save all permanent robot changes in the Xacro, URDF, or Gazebo configuration files.
- Save all permanent environment changes in the `.world` file.
- UI position and rotation changes are temporary unless the relevant Gazebo world or model is saved.
- Keep terminal commands aligned with the package names, launch filenames, node names, topics, and services used by the actual project.
