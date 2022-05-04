/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

// Warping Image Parameter
float widthA, widthB, heightA, heightB;
float maxWidth, maxHeight;

// HoughLinesP Parameter
double rho = 1;
double theta = CV_PI / 180;
int threshold = 50;
double minLineLength = 100;
double maxLineGap = 100;

// MAKING ROI MASK
cv::Mat regionOfInterset(cv::Mat img, cv::Point *vertices) {
  cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC3);

  const cv::Point *ppt[1] = {vertices};
  int npt[] = {4};

  cv::fillPoly(mask, ppt, npt, 1, cv::Scalar::all(255), cv::LINE_8);

  cv::Mat dst;
  cv::bitwise_and(img, mask, dst);

  return dst;
}

// FIND WHITE COLOR(ROAD LINE)
cv::Mat findColorHSV(cv::Mat img) {
  cv::Mat HSV;
  cv::cvtColor(img, HSV, cv::COLOR_BGR2HSV);

  cv::Mat mask_white = cv::Mat::zeros(img.size(), CV_8UC1);
  cv::Scalar lower_white = cv::Scalar(0, 0, 200);
  cv::Scalar upper_white = cv::Scalar(180, 50, 255);
  cv::inRange(HSV, lower_white, upper_white, HSV);

  return HSV;
}

// CALCULATING MAX WIDTH, MAX HEIGHT SIZE FOR WARP
void maxValueCalculator(cv::Point2f srcPt[4]) {
  widthA = sqrt((pow((srcPt[0].x - srcPt[1].x), 2)) +
                (pow((srcPt[0].y - srcPt[1].y), 2)));
  widthB = sqrt((pow((srcPt[2].x - srcPt[3].y), 2)) +
                (pow((srcPt[2].y - srcPt[3].y), 2)));
  maxWidth = std::max(static_cast<int>(widthA), static_cast<int>(widthB));

  heightA = sqrt((pow((srcPt[0].x - srcPt[3].x), 2)) +
                 (pow((srcPt[0].y - srcPt[3].y), 2)));
  heightB = sqrt((pow((srcPt[1].x - srcPt[2].x), 2)) +
                 (pow((srcPt[1].y - srcPt[2].y), 2)));
  maxHeight = std::max(static_cast<int>(heightA), static_cast<int>(heightB));
}

// DETECTING EDGE LINES
// SET line_row_theshold, line_high_threshold
// GOT A LINE VALUE WITHIN THE RANGE OF SLOPE
float edgeLines(cv::Mat img, const cv::Mat &line_result,
                std::vector<cv::Vec4i> lines) {
  int width = 800;
  float rslope, lslope = 0;
  int l_x1, l_x2, r_x1, r_x2 = 0;
  int line_flag[2] = {0};
  for (size_t i = 0; i < lines.size(); i++) {
    cv::Vec4i line = lines[i];
    int x1 = line[0];
    int y1 = line[1];
    int x2 = line[2];
    int y2 = line[3];

    float slope;
    if (x2 == x1) {
      slope = 999.;
    } else {
      slope = (y1 - y2) / static_cast<float>(x1 - x2);
    }

    float line_row_theshold = tan(15 * CV_PI / 180);
    float line_high_threshold = tan(89.9 * CV_PI / 180);

    if (line_row_theshold < fabsf(slope) &&
        fabsf(slope) < line_high_threshold) {
      if (x1 < width / 2) {
        // left side edge
        cv::line(line_result, cv::Point(x1, y1), cv::Point(x2, y2),
                 cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
        lslope = slope;
        l_x1 = x1;
        l_x2 = x2;
        line_flag[0] = 1;
        // printf("left %f", lslope );
      } else {
        // right side edge
        cv::line(line_result, cv::Point(x1, y1), cv::Point(x2, y2),
                 cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        rslope = slope;
        r_x1 = x1;
        r_x2 = x2;
        line_flag[1] = 1;
        // printf("right %f\n", rslope);
      }
    }
  }

  float servo_direct = (lslope + rslope) / 2;
  int c_x1 = 400;
  int c_y1 = 600;
  int c_y2 = 0;
  float c_x2 = 0;
  float diff = 0;

  printf("%d,%d\n", l_x2, r_x1);

  if (line_flag[0] == 1 && line_flag[1] == 1) {
    c_x2 = ((l_x2 + r_x1) / 2);
  } else {
    c_x2 = (((600 - 400 * servo_direct) / servo_direct) * (-1));
  }

  diff = (c_x2 - 400) * 0.8;
  servo_direct = diff;

  cv::line(line_result, cv::Point(c_x1, c_y1), cv::Point(c_x2, c_y2),
           cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

  return servo_direct;
}

int main() {
  cv::VideoCapture cap(2);
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

    // ROI - RECTANGULAR POINT
    cv::Point rect[4];
    rect[0] = cv::Point(0, 400);
    rect[1] = cv::Point(0, 600);
    rect[2] = cv::Point(800, 600);
    rect[3] = cv::Point(800, 400);

    cv::Mat ROI = regionOfInterset(src, rect);
    cv::Mat HSV = findColorHSV(ROI);

    cv::Mat filtering, closing;
    cv::bilateralFilter(HSV, filtering, 5, 100, 100);
    cv::morphologyEx(filtering, closing, cv::MORPH_CLOSE, cv::Mat());

    cv::Mat canny;
    cv::Canny(closing, canny, 150, 270);

    // HOUGH TRANSFORM
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(canny, lines, rho, theta, threshold, minLineLength,
                    maxLineGap);

    cv::Mat line_result = cv::Mat::zeros(src.size(), CV_8UC3);
    float direction = edgeLines(src, line_result, lines);

    cv::Mat result;
    cv::addWeighted(line_result, 1, src, 0.6, 0., result);
    cv::imshow("RESULT", result);
  }
  return 0;
}
