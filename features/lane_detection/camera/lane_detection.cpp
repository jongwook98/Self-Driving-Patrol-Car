/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include "lane_detection/camera/lane_detection.h"

// using namespace laneDetection;

cv::Mat LaneDetection::RegionOfInterset(cv::Mat img, cv::Point *vertices) {
    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC3);

    const cv::Point* ppt[1] = { vertices };
    int npt[] = { 4 };

    cv::fillPoly(mask, ppt, npt, 1, cv::Scalar::all(255), cv::LINE_8);
    cv::Mat dst;
    cv::bitwise_and(img, mask, dst);

    return dst;
}
cv::Mat LaneDetection::FindColorHsv(cv::Mat img) {
    cv::Mat HSV;
    cv::cvtColor(img, HSV, cv::COLOR_BGR2HSV);

    cv::Mat mask_white = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Scalar lower_white = cv::Scalar(0, 0, 200);
    cv::Scalar upper_white = cv::Scalar(180, 50, 255);
    cv::inRange(HSV, lower_white, upper_white, HSV);

    return HSV;
}

float LaneDetection::EdgeLines(cv::Mat img, const cv::Mat &line_result,
                                 std::vector<cv::Vec4i> lines) {
    for ( size_t i = 0; i < lines.size(); i++ ) {
        cv::Vec4i line = lines[i];
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];

       // float slope;
        if ( x2 == x1 )
            slope = 999.;
        else
            slope = (y1 - y2) / static_cast<float>(x1 - x2);

        float line_row_theshold = tan(15*CV_PI/180);
        float line_high_threshold = tan(89.9*CV_PI/180);

        if (line_row_theshold < fabsf(slope) &&
            fabsf(slope) < line_high_threshold) {
            if ( x1 < width / 2 ) {
                // left side edge
                cv::line(line_result, cv::Point(x1, y1), cv::Point(x2, y2),
                         cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
                l_slope = slope;
                l_x1 = x1;
                l_x2 = x2;
                line_flag[0] = 1;
            } else {
// right side edge
                cv::line(line_result, cv::Point(x1, y1), cv::Point(x2, y2),
                         cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
                r_slope = slope;
                r_x1 = x1;
                r_x2 = x2;
                line_flag[1] = 1;
            }
        }
    }
    float servo_direct = (l_slope+r_slope)/2;
    LaneDetection cal;
    return cal.CalculationAngle(servo_direct, line_flag,
                                  line_result, l_x2, r_x2);
}

float LaneDetection::CalculationAngle(float servo_direct, int line_flag[],
                  const cv::Mat &line_result, int l_x2, int r_x1) {
    if (line_flag[0] == 1 && line_flag[1] == 1) {
        c_x2 = ((l_x2 + r_x1)/2);
    } else {
        c_x2 = (((600-400*servo_direct)/servo_direct)*(-1));
    }
    diff = (c_x2-400);
    steering_angle = tan(diff/600)*10;
    if (steering_angle > 45)
        steering_angle = 45;
    else if (steering_angle < -45)
        steering_angle = -45;

    cv::line(line_result, cv::Point(c_x1, c_y1), cv::Point(c_x2, c_y2),
             cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

    return steering_angle;
}
