cmake_minimum_required(VERSION 3.11.0)

# set the project name
project(lane_detection VERSION 0.0.1 LANGUAGES CXX)

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
  ${CMAKE_SOURCE_DIR}/common/message_queue.cpp
  ${CMAKE_SOURCE_DIR}/camera/lane_detection.cpp
  ${CMAKE_SOURCE_DIR}/camera/traffic_light.cpp
)

# set the library informatio for the target
target_link_libraries(${PROJECT_NAME} PRIVATE rt ${OpenCV_LIBS})
