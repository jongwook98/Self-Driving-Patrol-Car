/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lane_detect/lane_detect.h>

LaneDetect::LaneDetect()
    : lane_detect_mutex(), mqtt(std::make_unique<Mqtt>(LANE_DETECT_PUB_TOPIC,
                                                       DEFAULT_PORT, nullptr)),
      mq(std::make_unique<MessageQueue>(LANE_PATH, MQ_SIZE) ) {}

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
    rect[0] = cv::Point(INIT, MID);
    rect[1] = cv::Point(INIT, HEIGHT);
    rect[2] = cv::Point(WIDTH, HEIGHT);
    rect[3] = cv::Point(WIDTH, MID);

    cv::rectangle(src, cv::Rect(rect[0], rect[2]),
                  BLUE, LINE_THICKNESS, LINE_TYPE, LINE_SHIFT);

    cv::Mat ROI = SetRoi(src, rect);
    cv::Mat HSV = FindColorHsv(ROI);

    cv::Mat filtering, closing;
    cv::bilateralFilter(HSV, filtering, PIXEL_D, SIGMACOLOR, SIGMASPACE);
    cv::morphologyEx(filtering, closing, cv::MORPH_CLOSE, cv::Mat());

    cv::Mat canny;
    cv::Canny(closing, canny, CANNY_LOW_THRESHOLD, CANNY_HIGH_THRESHOLD);
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
    l_x = 0;
    r_x = 0;
    stopline = -1;
    float servo_direct = 0;
    servo_direct = DivideLane(lines, line_result);
    return CalAngle(servo_direct, line_result);
}

float LaneDetect::DivideLane(std::vector<cv::Vec4i> lines,
                             const cv::Mat &line_result) {
    for ( size_t i = 0; i < lines.size(); i++ ) {
        cv::Vec4i line = lines[i];
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        float slope = 0;
        if ( x2 == x1 )
            slope = MAX_SLOPE;
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
        if (fabsf(slope) < LINE_ROW_THRESHOLD && (x2 - x1 > 20) &&
            x1 > HO_BOUNDARY && x1 < WIDTH / 2) {
            int mid = (y1 + y2) / 2;
            stopline = (WIDTH-static_cast<double>(mid)) * 0.18;
        } else {
            stopline = -1;
        }
    }
    return ( l_slope + r_slope ) / 2;
}

int LaneDetect::CalAngle(float servo_direct, const cv::Mat &line_result) {
    int calculated_angle = 0;
    int cal_point = FindLastPoint(servo_direct, line_result);
    int diff = (cal_point-(WIDTH/2));
    calculated_angle = atan2(diff, HEIGHT)*180/CV_PI;
    if (calculated_angle > MID_ANGLE)
        calculated_angle = MID_ANGLE;
    else if (calculated_angle < -MID_ANGLE)
        calculated_angle = -MID_ANGLE;
    return calculated_angle;
}

int LaneDetect::FindLastPoint(float servo_direct, const cv::Mat &line_result) {
    int c_x1 = WIDTH/2;
    int c_x2 = 0;
    int c_y1 = HEIGHT;
    int c_y2 = 0;
    if (line_flag[0] == 1 && line_flag[1] == 1) {
        if (fabsf(r_x - l_x) > HO_BOUNDARY) {
            if (l_x < left_line) {
                change_line = 1;
                l_x = l_x + X_HO_THRESHOLD;
            } else if (r_x > right_line) {
                change_line = 2;
                r_x = r_x - X_HO_THRESHOLD;
            } else {}
        }
        c_x2 = (l_x + r_x)/2;
    } else if (line_flag[0] == 1 || line_flag[1] == 1) {
        if (l_slope < -SLOPE_THRESHOLD) {
            r_x = l_x + X_THRESHOLD;
            c_x2 = (l_x + r_x)/2;
        } else if (r_slope > SLOPE_THRESHOLD) {
            l_x = r_x - X_THRESHOLD;
            c_x2 = (l_x + r_x)/2;
        } else {
            c_x2 = ((HEIGHT/servo_direct) * (-1))*0.5 + WIDTH/2;
        }
    } else {
        c_x2 = pre_c_x2;
    }
    pre_c_x2 = c_x2;
    cv::line(line_result, cv::Point(c_x1, c_y1), cv::Point(c_x2, c_y2),
             RED, LINE_THICKNESS, cv::LINE_AA);
    return c_x2;
}

int LaneDetect::SendMQ(int angle) {
    uint8_t data[11]= {0};
    int64_t stop_inf = 0;
    stopline = stopline * STOP_NUM;
    stop_inf = static_cast<int64_t>(stopline);
    for ( int i = 0; i< 8; i++ ) {
        data[i+2] = ((stop_inf >> (i * 8)) & 0xff);
    }
    data[1] = static_cast<uint8_t>(angle+90);
    data[10] = static_cast<uint8_t>(change_line);
    struct lane_message_q lane_mq;
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
