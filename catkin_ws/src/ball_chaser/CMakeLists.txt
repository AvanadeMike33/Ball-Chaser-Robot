cmake_minimum_required(VERSION 3.0.2)
project(ball_chaser)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  sensor_msgs
  message_generation
)

add_service_files(
  FILES
  DriveToTarget.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS
  roscpp
  std_msgs
  geometry_msgs
  sensor_msgs
  message_runtime
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

# Server node
add_executable(
  drive_bot
  src/drive_bot.cpp
)

add_dependencies(
  drive_bot
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries(
  drive_bot
  ${catkin_LIBRARIES}
)

# Camera client node
add_executable(
  process_image
  src/process_image.cpp
)

add_dependencies(
  process_image
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries(
  process_image
  ${catkin_LIBRARIES}
)
