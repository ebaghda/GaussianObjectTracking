#pragma once
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>

void calculateMoments(cv::Mat img, std::vector<cv::KeyPoint> keypoints)
{
  // Moments: 
  int patchSize = 45;  // Size of square patch (must be odd for symmetry)

  for (const auto& kp : keypoints) {
    // Get keypoint coordinates
    int x = static_cast<int>(kp.pt.x);
    int y = static_cast<int>(kp.pt.y);

    // Define ROI bounds, ensuring we don't go out of image bounds
    int halfSize = patchSize / 2;
    int x0 = std::max(0, x - halfSize);
    int y0 = std::max(0, y - halfSize);
    int x1 = std::min(img.cols, x + halfSize + 1);
    int y1 = std::min(img.rows, y + halfSize + 1);

    // Extract ROI
    cv::Rect roi(x0, y0, x1 - x0, y1 - y0);
    cv::Mat patch = img(roi).clone();  // Clone to get independent data

    // Optional: show or save patch
    cv::imshow("patch", patch);

    // Now you can analyze the patch (e.g., compute moments)
    cv::Moments m = cv::moments(patch, true);

    double xbar = m.m10 / m.m00;
    double ybar = m.m01 / m.m00;
    double sigmax = std::sqrt(m.mu20 / m.m00);
    double sigmay = std::sqrt(m.mu02 / m.m00);

    std::cout << "Patch at (" << x << "," << y << "): "
      << "Centroid=(" << xbar << "," << ybar << "), "
      << "sigmax=" << sigmax << ", sigmay=" << sigmay << std::endl;
  }
}