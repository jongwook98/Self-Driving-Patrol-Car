/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/lane_detection.h>
#include <lane_detection/camera/traffic_light.h>

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

    cv::Scalar lower_white = cv::Scalar(0, 0, 200);
    cv::Scalar upper_white = cv::Scalar(180, 50, 245);
    cv::inRange(HSV, lower_white, upper_white, HSV);

    return HSV;
}

int LaneDetection::EdgeLines(cv::Mat img, const cv::Mat &line_result,
                             std::vector<cv::Vec4i> lines) {
    l_slope = 0;
    r_slope = 0;
    line_flag[0] = 0;
    line_flag[1] = 0;
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
                if (slope < 0) {
                    cv::line(line_result, cv::Point(x1, y1),
                             cv::Point(x2, y2),
                             cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
                    l_slope = slope;
                    l_x1 = x1;
                    l_x2 = x2;
                    line_flag[0] = 1;
                } else {}
            } else {
                // right side edge
                if (slope > 0) {
                    cv::line(line_result, cv::Point(x1, y1),
                             cv::Point(x2, y2),
                             cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
                    r_slope = slope;
                    r_x1 = x1;
                    r_x2 = x2;
                    line_flag[1] = 1;
                } else {}
            }
        }
    }
    float servo_direct = (l_slope+r_slope)/2;
    return CalculationAngle(servo_direct, line_flag,
                                  line_result, l_x2, r_x2);
}

int LaneDetection::CalculationAngle(float servo_direct, int line_flag[],
                  const cv::Mat &line_result, int l_x2, int r_x1) {
    c_x2 = 540;
    calculated_angle = 0;
    if (line_flag[0] == 1 && line_flag[1] == 1) {
        c_x2 = (l_x2 + r_x1)/2;
    } else if (line_flag[0] == 1 || line_flag[1] == 1) {
        if (l_slope < -1.2) {
            r_x1 = l_x2 + 780;
            c_x2 = (l_x2 + r_x1)/2;
        } else if (r_slope > 1.2) {
            l_x2 = r_x1 - 780;
            c_x2 = (l_x2 + r_x1)/2;
        } else {
        c_x2 = ((600/servo_direct) * (-1))*0.5 + 540;
        }
    } else {
        c_x2 = pre_c_x2;
    }
    pre_c_x2 = c_x2;

    diff = (c_x2-540);
    calculated_angle = atan2(diff, 600)*180/3.141591;
    if (calculated_angle > 90)
        calculated_angle = 90;
    else if (calculated_angle < -90)
        calculated_angle = -90;

    cv::line(line_result, cv::Point(c_x1, c_y1), cv::Point(c_x2, c_y2),
             cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

    return calculated_angle;
}

int LaneDetection::StraightLaneAngle(cv::Mat src, uint8_t turn_dir) {
    int straight_lane_angle = 0;

    printf("turn_dir : %d\n", turn_dir);
    printf("\n");

    cv::Point rect[4];
    rect[0] = cv::Point(0, 480);
    rect[1] = cv::Point(0, 600);
    rect[2] = cv::Point(1080, 600);
    rect[3] = cv::Point(1080, 480);

    cv::rectangle(src, cv::Rect(rect[0], rect[2]),
                  cv::Scalar(255, 0, 0), 2, 8, 0);

    if (turn_dir == 0) {
        straight_lane_angle = 0;
        cv::imshow("RESULT", src);
        return straight_lane_angle;
    } else if (turn_dir == 2) {
        straight_lane_angle = 0;
        cv::imshow("RESULT", src);
        return straight_lane_angle;
    } else if (turn_dir == 3) {
        straight_lane_angle = -30;
        cv::imshow("RESULT", src);
        return straight_lane_angle;
    } else {
        cv::Mat ROI = RegionOfInterset(src, rect);
        cv::Mat HSV = FindColorHsv(ROI);

        cv::Mat filtering, closing;
        cv::bilateralFilter(HSV, filtering, 5, 100, 100);
        cv::morphologyEx(filtering, closing, cv::MORPH_CLOSE, cv::Mat());

        cv::Mat canny;
        cv::Canny(closing, canny, 150, 270);

        // HOUGH TRANSFORM
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(canny, lines, rho, theta, threshold,
                        min_line_length, max_line_gap);

        cv::Mat line_result = cv::Mat::zeros(src.size(), CV_8UC3);
        straight_lane_angle = EdgeLines(src, line_result, lines);

        cv::Mat result;
        cv::addWeighted(line_result, 1, src, 0.6, 0., src);
        cv::imshow("RESULT", src);
        return straight_lane_angle;
    }
}
