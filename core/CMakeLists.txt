cmake_minimum_required(VERSION 3.11.0)

# set the project name
project(sdpc VERSION 0.0.1 LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(
  SF_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/\
MATLAB-SIMULINK-code-generation/sf/sf_simulink_ert_rtw"
)

# set the variable
set(
  SF_SIMULINK_SRC
  "${SF_DIR}/rtGetInf.c"
  "${SF_DIR}/rtGetNaN.c"
  "${SF_DIR}/rt_nonfinite.c"
  "${SF_DIR}/sf_simulink.c"
  "${SF_DIR}/ert_main.c"
)

set(
  SDPC_SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/common/sdpc_common.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/sdpc_main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/sdpc_msgque.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/sdpc_uart.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/lane_recognition/sdpc_lane.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/sensor_fusion/sdpc_sensor_fusion.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_recognition/sdpc_vehicle.c"
)
set(SDPC_SHARED_LIB ${PROJECT_NAME}_shared)

# set the header information for the target and *.so
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${SF_DIR}
)

# make exe file
add_executable(${PROJECT_NAME} ${SDPC_SRC})

# set the compile option for the target
target_compile_options(${PROJECT_NAME} PRIVATE -W -Wall -g -ggdb -fPIC)

# set the library information for the target
target_link_libraries(${PROJECT_NAME} PRIVATE pthread rt sf_shared)

# set the macro for the target
target_compile_definitions(${PROJECT_NAME} PRIVATE -D_STANDALONE_VERSION)

# make *.so
add_library(${SDPC_SHARED_LIB} SHARED ${SDPC_SRC})
add_library(sf_shared SHARED ${SF_SIMULINK_SRC})

# set the link option for the *.so
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

# set the library information for the *.so
target_link_libraries(${SDPC_SHARED_LIB} PRIVATE pthread rt sf_shared)
