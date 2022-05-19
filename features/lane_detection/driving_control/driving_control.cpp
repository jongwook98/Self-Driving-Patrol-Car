/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include "lane_detection/driving_control/driving_control.h"

// using namespace laneDetection;

uint8_t DrivingControl::DrivingStatusFlag(int light_color) {
    if (stop_flag == 1) {  // wait for red
        if (stop_cnt >= stop_time) {
            stop_flag = 0;
            stop_cnt = 0;
        } else {
            force_flag = 0;
            driving_status_flag = 0;
            stop_flag = 1;
            stop_cnt++;
        }
    } else {
        if (light_color == 1) {  // red | stop
            force_flag = 0;
            driving_status_flag = 0;
            stop_flag = 1;
        } else if (light_color == 2) {  // left green | left turn
            force_flag = 1;
            driving_status_flag = 111;
        } else if (light_color == 3) {  // green | straight turn
            force_flag = 1;
            driving_status_flag = 100;
        } else {  // nothing | go
            force_flag = 0;
            driving_status_flag = 100;
        }
    }
    printf("stop_flag : %d\n", stop_flag);
    return driving_status_flag;
}

int DrivingControl::SteeringAngle(cv::Mat img, uint8_t status) {
    LaneDetection detect;

    if (left_stay_flag == 1 && left_stay_cnt <= left_stay_time) {
        // delay left_stay_time during left turn
        steering_angle = detect.StraightLaneAngle(img, 3);
        left_stay_cnt++;
    } else if (force_straight_stay_flag == 1 &&
               force_straight_stay_cnt <= force_straight_stay_time) {
        // delay force_straight_stay_time during force straight
        steering_angle = detect.StraightLaneAngle(img, 2);
        force_straight_stay_cnt++;
    } else {
        left_stay_flag = 0;
        left_stay_cnt = 0;
        force_straight_stay_flag = 0;
        force_straight_stay_cnt = 0;

        if (force_flag == 1) {
            if (driving_status_flag == 111) {
                left_flag = 1;
                left_stay_flag = 1;
            } else if (driving_status_flag == 100) {
                force_straight_flag = 1;
                force_straight_stay_flag = 1;
            } else {}

            if (left_flag == 1) {  // left turn
                steering_angle = detect.StraightLaneAngle(img, 3);
            } else if (force_straight_flag == 1) {   // force straight
                steering_angle = detect.StraightLaneAngle(img, 2);
            } else {   // go
                steering_angle = detect.StraightLaneAngle(img, 1);
            }
        } else {
            if (driving_status_flag == 0) {  // stop
                steering_angle = detect.StraightLaneAngle(img, 0);
            } else {  // go
                steering_angle = detect.StraightLaneAngle(img, 1);
            }
        }
    }

    return steering_angle;
}
