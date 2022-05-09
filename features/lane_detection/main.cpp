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
    Lane val;
    MessageQueue *MQ;
    MQ = new MessageQueue(val.mq_path[0], val.mq_msg_size[0]);
    cv::VideoCapture cap(0);
    // cv::VideoCapture cap(cv::CAP_V4L2);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 800);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
    if (!cap.isOpened()) {
        printf("Can't open the camera");
        return -1;
    }

    cv::Mat src;
    while (1) {
        cap >> src;
        if (cv::waitKey(1) == 27)
             break;
        val.data[0] = static_cast<uint8_t>(light.FindTrafficLight(src));

        cv::Point rect[4];
        rect[0] = cv::Point(0, 400);
        rect[1] = cv::Point(0, 600);
        rect[2] = cv::Point(800, 600);
        rect[3] = cv::Point(800, 400);

        cv::Mat ROI = detect.RegionOfInterset(src, rect);
        cv::Mat HSV = detect.FindColorHsv(ROI);

        cv::Mat filtering, closing;
        cv::bilateralFilter(HSV, filtering, 5, 100, 100);
        cv::morphologyEx(filtering, closing, cv::MORPH_CLOSE, cv::Mat());

        cv::Mat canny;
        cv::Canny(closing, canny, 150, 270);

        // HOUGH TRANSFORM
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(canny, lines, val.rho, val.theta, val.threshold,
                        val.min_line_length, val.max_line_gap);

        cv::Mat line_result = cv::Mat::zeros(src.size(), CV_8UC3);
        int st_angle = static_cast<int>(detect.EdgeLines(src,
                                        line_result, lines));
        val.data[1] = static_cast<uint8_t>(st_angle+90);

        cv::Mat result;
        cv::addWeighted(line_result, 1, src, 0.6, 0., result);
        cv::imshow("RESULT", result);
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
    }

    return 0;
}
