/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/traffic_light/traffic_light.h>

TrafficLight::TrafficLight()
  : traffic_light_mutex(),
    mqtt(std::make_unique<Mqtt>(TRAFFIC_LIGHT_PUB_TOPIC, DEFAULT_PORT,
                                nullptr)),
    mq(std::make_unique<MessageQueue>(TRAFFIC_PATH, T_MQ_SIZE)) {}

cv::Mat TrafficLight::RegionOfInterset(cv::Mat cam) {
  cv::Mat mask = cv::Mat::zeros(cam.size(), CV_8UC3);

  cv::Point traffic_rect[4];
  traffic_rect[0] = cv::Point(0, 0);
  traffic_rect[1] = cv::Point(0, TRAFFIC_HEIGHT);
  traffic_rect[2] = cv::Point(TRAFFIC_WIDTH, TRAFFIC_HEIGHT);
  traffic_rect[3] = cv::Point(TRAFFIC_WIDTH, 0);

  const cv::Point* ppt[1] = { traffic_rect };
  int npt[] = { 4 };

  cv::fillPoly(mask, ppt, npt, 1, cv::Scalar::all(255), cv::LINE_8);
  cv::Mat dst;
  cv::bitwise_and(cam, mask, dst);

  return dst;
}

void TrafficLight::ShowResult(cv::Mat result, int color) {
  cv::Point text_point;
  text_point.x = 40;
  text_point.y = 40;

  cv::putText(result, std::to_string(color), text_point, font_face,
              font_scale, cv::Scalar::all(255));
  cv::imshow("TRAFFICRESULT", result);
}

int TrafficLight::TotalPixel(std::vector<cv::Vec3f> circles, size_t i) {
  int radius = circles[i][2];
  int total_pixel = cvFloor(3.14159*radius*radius);

  return total_pixel;
}

int TrafficLight::CountGreen(cv::Mat cam,
                             std::vector<cv::Vec3f> circles, size_t i) {
  int center_x = circles[i][0];
  int center_y = circles[i][1];
  int width = 2*circles[i][2];
  int green_data, red_data, blue_data;
  int green_cnt = 0;

  for (int n = center_x - width; n <= center_x + width; n++) {
    for (int m = center_y - width; m <= center_y + width; m++) {
      blue_data = cam.at<cv::Vec3b>(m, n)[0];
      green_data = cam.at<cv::Vec3b>(m, n)[1];
      red_data = cam.at<cv::Vec3b>(m, n)[2];

      if (green_data - blue_data > 20 && green_data - red_data > 20
          && green_data > 100) {
        green_cnt++;
      }
    }
  }
  return green_cnt;
}

int TrafficLight::GetColor(cv::Mat cam, cv::Mat gray_img,
                           std::vector<cv::Vec3f> circles, size_t i) {
  cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
  int radius_round = cvRound(circles[i][2]);
  int k = circles[i][0];   // x
  int j = circles[i][1];   // y
  int pixel_blue = cam.at<cv::Vec3b>(j, k)[0];
  int pixel_green = cam.at<cv::Vec3b>(j, k)[1];
  int pixel_red = cam.at<cv::Vec3b>(j, k)[2];

  cv::circle(gray_img, center, 3, cv::Scalar(0, 0, 255), -1, 8, 0);
  cv::circle(gray_img, center, radius_round,
             cv::Scalar(0, 0, 255), 3, 8, 0);

  if (pixel_red - pixel_blue > 20 && pixel_red - pixel_green > 20 &&
      pixel_red > 100) {  // red light
    return 1;
  } else if (pixel_green - pixel_red > 20 && pixel_green - pixel_blue > 20
             && pixel_green > 100) {  // green light
    int total_pixel = 0;
    int green_cnt = 0;

    total_pixel = TotalPixel(circles, i);
    green_cnt = CountGreen(cam, circles, i);

    if (total_pixel * G_RATIO < green_cnt) {  // circle green
      return 2;
    } else {  // sign green
      return 3;
    }
  } else {  // nothing
    return 0;
  }
}

void TrafficLight::Operate(cv::Mat cam) {
  int light_color = 0;  // nothing, red, green, signed green

  cv::Mat traffic_roi = RegionOfInterset(cam);
  cv::Mat gray_img, filtered_image;
  cv::cvtColor(traffic_roi, gray_img, CV_BGR2GRAY);
  cv::bilateralFilter(gray_img, filtered_image, -1, sigma_color, sigma_space);

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(filtered_image, circles, CV_HOUGH_GRADIENT,
                   2, 50, 200, 70, 10, 30);

  for (size_t i = 0; i < circles.size(); i++) {
    light_color = GetColor(cam, gray_img, circles, i);
  }

  ShowResult(gray_img, light_color);

  Publisher(light_color);
}

int TrafficLight::Publisher(int color) {
  uint8_t data[2]= {0, };
  data[1] = static_cast<uint8_t>(color);
  struct message_q traffic_mq;
  mq->BuildMessage(reinterpret_cast<uint8_t *>(&traffic_mq), MqMode::OPENCV,
                    data, sizeof(traffic_mq), sizeof(data));
  mq->Send(reinterpret_cast<const char *>(&traffic_mq), sizeof(traffic_mq));
  return 0;
}

void *TrafficLight::Run(void *arg) {
  const std::string str = TRAFFIC_LIGHT_PUB_TOPIC;
  std::unique_lock<std::mutex> sync(traffic_light_mutex, std::defer_lock);
  cv::Mat cam;
  mqtt->Subscribe(TRAFFIC_LIGHT_SUB_TOPIC, read_buf, sizeof(read_buf));
  while (status) {
    DEBUG_MSG("[Traffic Light] in Thread!");
    /* publish to the camera */
    mqtt->Publish(TRAFFIC_LIGHT_PUB_TOPIC, str.c_str(), str.length());
    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the lane detect */
      memcpy(&cam, read_buf, sizeof(cv::Mat));
      memset(read_buf, 0x0, sizeof(read_buf));
      Operate(cam);
    }
    sync.unlock();
  }
  return nullptr;
}
