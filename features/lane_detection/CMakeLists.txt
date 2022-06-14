cmake_minimum_required(VERSION 3.11.0)

# set the project name
project(lane_detection VERSION 1.0.1 LANGUAGES CXX)

# set the variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add the package of the OpenCV
find_package(OpenCV REQUIRED)

# add the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# make exe file
add_executable(
  ${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/main.cpp
  ${CMAKE_SOURCE_DIR}/camera/camera.cpp
  ${CMAKE_SOURCE_DIR}/common/message_queue.cpp
  ${CMAKE_SOURCE_DIR}/common/shared_memory.cpp
  ${CMAKE_SOURCE_DIR}/common/thread.cpp
  ${CMAKE_SOURCE_DIR}/core/core.cpp
  ${CMAKE_SOURCE_DIR}/lane_detect/lane_detect.cpp
  ${CMAKE_SOURCE_DIR}/traffic_light/traffic_light.cpp
)

# set the library informatio for the target
target_link_libraries(${PROJECT_NAME} PRIVATE pthread rt ${OpenCV_LIBS})
