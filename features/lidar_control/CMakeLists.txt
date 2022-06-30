cmake_minimum_required(VERSION 3.11.0)

project(lidar_control VERSION 1.0.0 LANGUAGES CXX)

find_package(Git)

if(Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-pars --show-toplevel
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE ROOT_DIR
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(ROOT_DIR "../..")
endif()
include_directories(${ROOT_DIR}/third_party/rplidar_sdk/sdk/include)
link_directories(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/output/Linux/Release")

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/mqueue_receive/server.c
    ${CMAKE_SOURCE_DIR}/ultra_simple/main.cpp
)

target_link_libraries(
    ${PROJECT_NAME} rplidar_sdk
)
