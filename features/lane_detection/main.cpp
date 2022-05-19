/*
 * Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved
 *
 *
 */
#include <lane_detection/main.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <cstring>

int main() {
    LaneDetection detect;
    TrafficLight light;
    DrivingControl control;
    Lane val;
    MessageQueue *MQ;
    MQ = new MessageQueue(val.mq_path[0], val.mq_msg_size[0]);
    // cv::VideoCapture cap(0);
    cv::VideoCapture cap(cv::CAP_V4L2);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1080);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
    if (!cap.isOpened()) {
        printf("Can't open the camera");
        return -1;
    }

    cv::Mat cam;
    while (1) {
        cap >> cam;
        if (cv::waitKey(1) == 27)
             break;

        val.traffic_light = light.FindTrafficLight(cam);
        val.driving_status_flag = control.DrivingStatusFlag(
                                                 val.traffic_light);
        val.steering_angle = control.SteeringAngle(cam,
                                             val.driving_status_flag);
        val.data[0] = val.driving_status_flag;
        val.data[1] = val.steering_angle + 90;

        struct message_q lane_mq;
        MQ->BuildMessage(&lane_mq.start, MqMode::OPENCV,
                         val.data,
                         sizeof(lane_mq),
                         sizeof(val.data));
        char send_msg[5]={static_cast<char>(lane_mq.start),
                          static_cast<char>(lane_mq.mode),
                          static_cast<char>(lane_mq.status_flag),
                          static_cast<char>(lane_mq.angle),
                          static_cast<char>(lane_mq.stop)};
        MQ->Send(send_msg, val.mq_msg_size[0]);

        if (val.traffic_light == 1) {
            printf("T_light : red_light  | ");
        } else if (val.traffic_light == 2) {
            printf("T_light : green_light| ");
        } else if (val.traffic_light == 3) {
            printf("T_light : left_light | ");
        } else {
            printf("T_light : nothing    | ");
        }
        printf("status_flag : %d | ", val.driving_status_flag);
        printf("steering_angle : %d\n", val.steering_angle);
    }

    return 0;
}
