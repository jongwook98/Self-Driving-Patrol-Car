cmake_minimum_required(VERSION 3.11.0)

# set the project name
project(lane_detection VERSION 1.0.1 LANGUAGES CXX)

# set the variables
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add the packages
find_package(Git)
find_package(OpenCV REQUIRED)
find_package(glog 0.6.0 REQUIRED PATHS "${CMAKE_SOURCE_DIR}/out/glog")

# add the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# for mosquitto
if(Git_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --show-toplevel
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE ROOT_DIR
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(ROOT_DIR "../..")
endif()

include_directories(${ROOT_DIR}/third_party/mosquitto/include)
link_directories(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/out/mosquitto/lib")

# for rplidar sdk
set(LIDAR_DIR "${ROOT_DIR}/third_party/rplidar_sdk")
include_directories(${LIDAR_DIR}/sdk/include)
link_directories(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/out/lidar")

# make exe file
add_executable(
  ${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/main.cpp
  ${CMAKE_SOURCE_DIR}/camera/camera.cpp
  ${CMAKE_SOURCE_DIR}/common/message_queue.cpp
  ${CMAKE_SOURCE_DIR}/common/mqtt.cpp
  ${CMAKE_SOURCE_DIR}/common/shared_memory.cpp
  ${CMAKE_SOURCE_DIR}/common/thread.cpp
  ${CMAKE_SOURCE_DIR}/core/core.cpp
  ${CMAKE_SOURCE_DIR}/lane_detect/lane_detect.cpp
  ${CMAKE_SOURCE_DIR}/lidar/lidar.cpp
  ${CMAKE_SOURCE_DIR}/traffic_light/traffic_light.cpp
)

# set the library informatio for the target
target_link_libraries(
  ${PROJECT_NAME} PRIVATE pthread rt ${OpenCV_LIBS} glog::glog mosquitto
  libsl_lidar_sdk.a
)
