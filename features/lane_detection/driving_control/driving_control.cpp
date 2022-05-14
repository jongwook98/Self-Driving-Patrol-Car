/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include "lane_detection/driving_control/driving_control.h"

// using namespace laneDetection;

uint8_t DrivingControl::DrivingStatusFlag(int light_color) {
    if (stop_flag == 1) {  // wait for red
        if (stop_cnt >= 1000000) {
            stop_flag = 0;
            stop_cnt = 0;
        } else {
            force_flag = 0;
            driving_status_flag = 0;
            stop_flag = 1;
            stop_cnt++;
        }
    } else {
        if (light_color == 1) {  // stop
            force_flag = 0;
            driving_status_flag = 0;
            stop_flag = 1;
        } else if (light_color == 2) {  // left turn
            force_flag = 1;
            driving_status_flag = 111;
        } else if (light_color == 3) {  // straight turn
            force_flag = 1;
            driving_status_flag = 100;
        } else {  // go
            force_flag = 0;
            driving_status_flag = 100;
        }
    }
    return driving_status_flag;
}

int DrivingControl::SteeringAngle(cv::Mat img, uint8_t status) {
    LaneDetection detect;

    if (left_stay_flag == 1 && left_stay_cnt <= 100000) {
        // delay 100000 during left turn
        left_stay_cnt++;
    } else if (force_straight_stay_flag == 1 &&
               force_straight_stay_cnt <= 100000) {
        // delay 100000 during force straight
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

            if (left_flag == 1) {
                steering_angle = -30;
            } else if (force_straight_flag == 1) {
                steering_angle = 0;
            } else {
                steering_angle = detect.StraightLaneAngle(img);
            }
        } else {
            if (driving_status_flag == 0) {
                steering_angle = 0;
            } else {  // driving_status_flag == 100
                steering_angle = detect.StraightLaneAngle(img);
            }
        }
    }

    return steering_angle;
}
