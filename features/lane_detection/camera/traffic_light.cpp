/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include "lane_detection/camera/traffic_light.h"

uint8_t TrafficLight::FindTrafficLight(cv::Mat img) {
    LaneDetection traffic;

    cv::Point traffic_rect[4];
    traffic_rect[0] = cv::Point(540, 0);
    traffic_rect[1] = cv::Point(540, 300);
    traffic_rect[2] = cv::Point(1080, 300);
    traffic_rect[3] = cv::Point(1080, 0);

    cv::Mat traffic_roi = traffic.RegionOfInterset(img, traffic_rect);
    cv::rectangle(img, cv::Rect(traffic_rect[0], traffic_rect[2]),
                  cv::Scalar(0, 255, 0), 2, 8, 0);

    cv::Mat gray_img, filtered_image;
    cv::cvtColor(traffic_roi, gray_img, CV_BGR2GRAY);
    cv::GaussianBlur(gray_img, filtered_image, cv::Size(7, 7), 0);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(filtered_image, circles, CV_HOUGH_GRADIENT,
                     2, 50, 200, 70, 20, 35);

    light_color = 0;

    for (size_t i = 0; i < circles.size(); i++) {
        int k = circles[i][0];   // x
        int j = circles[i][1];   // y
        int r = circles[i][2];   // r
        int pixel_blue = img.at<cv::Vec3b>(j, k)[0];
        int pixel_green = img.at<cv::Vec3b>(j, k)[1];
        int pixel_red = img.at<cv::Vec3b>(j, k)[2];

        cv::Point center(cvRound(k), cvRound(j));
        int radius_round = cvRound(r);
        cv::circle(img, center, radius_round, cv::Scalar(0, 0, 255), 3, 8, 0);

        if (pixel_red-pixel_blue > 20 && pixel_red-pixel_green > 20 &&
            pixel_red > 100) {  // red light
            light_color = 1;
        } else if (pixel_green-pixel_red > 20 && pixel_green-pixel_blue > 20
                   && pixel_green > 100) {  // green light
            int center_x = circles[i][0];
            int center_y = circles[i][1];
            int radius = circles[i][2];
            int width = 2*circles[i][2];
            int green_data, red_data, blue_data;
            int green_cnt = 0;
            int total_pixel = cvFloor(3.14159*radius*radius);

            for (int n = center_x-width; n <= center_x+width; n++) {
                for (int m = center_y-width; m <= center_y+width; m++) {
                    blue_data = img.at<cv::Vec3b>(m, n)[0];
                    green_data = img.at<cv::Vec3b>(m, n)[1];
                    red_data = img.at<cv::Vec3b>(m, n)[2];

                    if (green_data-blue_data > 20 && green_data-red_data > 20
                        && green_data > 100) {
                        green_cnt++;
                    }
                }
            }
            if (total_pixel*0.6 < green_cnt) {  // circle green
                light_color = 2;
            } else {  // sign green
                light_color = 3;
            }
        } else {  // nothing
            light_color = 0;
        }
    }

    return light_color;
}
