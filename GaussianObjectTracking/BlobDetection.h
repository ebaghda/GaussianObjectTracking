#pragma once
#include <opencv2/imgproc.hpp>

std::vector<cv::KeyPoint> DetectBlobs(cv::VideoCapture cap, cv::Mat img)
{
  //Perform blob detection
// 2. Setup SimpleBlobDetector parameters
cv::SimpleBlobDetector::Params params;

// Change thresholds
params.minThreshold = 5;
params.maxThreshold = 255;

// Filter by Area.
params.filterByArea = true;
params.minArea = 5;
params.maxArea = 200;

// Filter by Circularity
params.filterByCircularity = false;
params.minCircularity = (float)0.8;

// Filter by Convexity
params.filterByConvexity = false;
params.minConvexity = (float)0.95;

// Filter by Inertia
params.filterByInertia = false;
params.minInertiaRatio = (float)0.01;

// 3. Create a SimpleBlobDetector object using the parameters
cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

// 4. Detect blobs
std::vector<cv::KeyPoint> keypoints;
detector->detect(img, keypoints);

return keypoints;
}


void updateImageWithDetectedObjects(cv::Mat img, std::vector<cv::KeyPoint> keypoints)
{
  cv::Mat image_with_keypoints;
  cv::cvtColor(img, image_with_keypoints, cv::COLOR_GRAY2BGR); // Convert to color for drawing
  cv::bitwise_not(image_with_keypoints, image_with_keypoints);
  cv::drawKeypoints(image_with_keypoints, keypoints, image_with_keypoints, cv::Scalar(0, 255, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  cv::imshow("Blobs Detected", image_with_keypoints);
}
