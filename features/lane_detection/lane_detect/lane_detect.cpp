/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lane_detect/lane_detect.h>

LaneDetect::LaneDetect()
    : lane_detect_mutex(), mqtt(std::make_unique<Mqtt>(LANE_DETECT_PUB_TOPIC,
                                                       DEFAULT_PORT, nullptr)),
      mq(std::make_unique<MessageQueue>(LANE_PATH, (sizeof(uint8_t)*14)) ) {}

int LaneDetect::CalLaneAngle(cv::Mat src) {
    int straight_lane_angle = 0;
    // CONVERT IMAGE
    cv::Mat canny = ConvertImg(src);
    // HOUGH TRANSFORM
    straight_lane_angle = FindLanes(src, canny);
    SendMQ(straight_lane_angle);
    return 0;
}

cv::Mat LaneDetect::ConvertImg(cv::Mat src) {
    cv::Point rect[4];
    rect[0] = cv::Point(0, 480);
    rect[1] = cv::Point(0, HEIGHT);
    rect[2] = cv::Point(WIDTH, HEIGHT);
    rect[3] = cv::Point(WIDTH, 480);

    cv::rectangle(src, cv::Rect(rect[0], rect[2]),
                  cv::Scalar(255, 0, 0), 2, 8, 0);

    cv::Mat ROI = SetRoi(src, rect);
    cv::Mat HSV = FindColorHsv(ROI);

    cv::Mat filtering, closing;
    cv::bilateralFilter(HSV, filtering, 5, 100, 100);
    cv::morphologyEx(filtering, closing, cv::MORPH_CLOSE, cv::Mat());

    cv::Mat canny;
    cv::Canny(closing, canny, 150, 270);
    return canny;
}

cv::Mat LaneDetect::SetRoi(cv::Mat img, cv::Point *vertices) {
    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC3);

    const cv::Point* ppt[1] = { vertices };
    int npt[] = { 4 };

    cv::fillPoly(mask, ppt, npt, 1, cv::Scalar::all(255), cv::LINE_8);
    cv::Mat dst;
    cv::bitwise_and(img, mask, dst);

    return dst;
}

cv::Mat LaneDetect::FindColorHsv(cv::Mat img) {
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower_white = LOWER_HSV;
    cv::Scalar upper_white = UPPER_HSV;
    cv::inRange(hsv, lower_white, upper_white, hsv);

    return hsv;
}

int LaneDetect::FindLanes(cv::Mat src, cv::Mat canny) {
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(canny, lines, RHO, THETA, THRESHOLD,
                    MIN_LINE_LENGTH, MAX_LINE_GAP);
    cv::Mat line_result = cv::Mat::zeros(src.size(), CV_8UC3);

    int straight_lane_angle = CalLanes(src, line_result, lines);
    cv::addWeighted(line_result, 1, src, 0.6, 0., src);
    cv::imshow("RESULT", src);
    return straight_lane_angle;
}

int LaneDetect::CalLanes(cv::Mat img, const cv::Mat &line_result,
                             std::vector<cv::Vec4i> lines) {
    l_slope = 0;
    r_slope = 0;
    line_flag[0] = 0;
    line_flag[1] = 0;
    stopline = -1;
    float servo_direct = 0;
    servo_direct = DivideLane(lines);
    return CalAngle(servo_direct, line_result);
}

float LaneDetect::DivideLane(std::vector<cv::Vec4i> lines) {
    for ( size_t i = 0; i < lines.size(); i++ ) {
        cv::Vec4i line = lines[i];
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        float slope = 0;
        if ( x2 == x1 )
            slope = 999.;
        else
            slope = (y1 - y2) / static_cast<float>(x1 - x2);

        if (LINE_ROW_THRESHOLD < fabsf(slope) &&
            fabsf(slope) < LINE_HIGH_THRESHOLD) {
            if ( x2 < WIDTH / 2 && slope < 0 ) {
                l_slope = slope;
                l_x = x2;
                line_flag[0] = 1;
            } else if ( x1 > WIDTH / 2 && slope > 0 ) {
                r_slope = slope;
                r_x = x1;
                line_flag[1] = 1;
            } else {}
        }
        if (fabsf(slope) < LINE_ROW_THRESHOLD && x2 - x1 > 20) {
            int mid = (y1 + y2) / 2;
            stopline = (600-static_cast<double>(mid));
        } else {
            stopline = -1;
        }
    }
    return ( l_slope + r_slope ) / 2;
}

int LaneDetect::CalAngle(float servo_direct, const cv::Mat &line_result) {
    float calculated_angle = 0;
    int cal_point = FindLastPoint(servo_direct, line_result);
    float diff = (cal_point-(WIDTH/2));
    calculated_angle = atan2(diff, HEIGHT)*180/CV_PI;
    if (calculated_angle > 90)
        calculated_angle = 90;
    else if (calculated_angle < -90)
        calculated_angle = -90;
    return calculated_angle;
}

int LaneDetect::FindLastPoint(float servo_direct, const cv::Mat &line_result) {
    int c_x1 = WIDTH/2;
    int c_x2 = 0;
    int c_y1 = HEIGHT;
    int c_y2 = 0;
    if (line_flag[0] == 1 && line_flag[1] == 1) {
        c_x2 = (l_x + r_x)/2;
    } else if (line_flag[0] == 1 || line_flag[1] == 1) {
        if (l_slope < -1.2) {
            r_x = l_x + 580;  // 780
            c_x2 = (l_x + r_x)/2;
        } else if (r_slope > 1.2) {
            l_x = r_x - 580;
            c_x2 = (l_x + r_x)/2;
        } else {
            c_x2 = ((HEIGHT/servo_direct) * (-1))*0.5 + WIDTH/2;
        }
    } else {
        c_x2 = pre_c_x2;
    }
    pre_c_x2 = c_x2;
    cv::line(line_result, cv::Point(c_x1, c_y1), cv::Point(c_x2, c_y2),
             cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
    return c_x2;
}

int LaneDetect::SendMQ(int angle) {
    uint8_t data[11]= {0};
    int64_t stop_inf = 0;
    stopline = stopline * 1000;
    stop_inf = static_cast<int64_t>(stopline);
    for ( int i = 0; i< 8; i++ ) {
        data[8-i] = ((stop_inf >> (i * 8)) & 0xff);
    }
    data[9] = static_cast<uint8_t>(angle+90);
    data[10] = 0;
    struct message_q lane_mq;
    mq->BuildMessage(reinterpret_cast<uint8_t *>(&lane_mq), MqMode::OPENCV,
                     data, sizeof(lane_mq), sizeof(data));
    mq->Send(reinterpret_cast<const char *>(&lane_mq), (sizeof(lane_mq)));
    return 0;
}

void *LaneDetect::Run(void *arg) {
  std::unique_lock<std::mutex> sync(lane_detect_mutex, std::defer_lock);
  cv::Mat cam;
  mqtt->Subscribe(LANE_DETECT_SUB_TOPIC, read_buf, sizeof(read_buf));
  while (status) {
    DEBUG_MSG("[Lane Detect] in Thread!");
    /* publish to the traffic light */
    mqtt->Publish(LANE_DETECT_PUB_TOPIC, reinterpret_cast<void *>(&cam),
                  sizeof(cam));
    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the camera */
      memcpy(&cam, read_buf, sizeof(cv::Mat));
      memset(read_buf, 0x0, sizeof(read_buf));
      CalLaneAngle(cam);
    }
    sync.unlock();
  }
  return nullptr;
}
