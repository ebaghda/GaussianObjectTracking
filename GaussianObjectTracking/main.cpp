#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "UpdateBrightnessAndContrast.h" // header file to allow for live, manual brightness and contrast control
#include <opencv2/features2d.hpp>
#include "GUIControls.h"
#include <iomanip>
#include <fstream>
#include <filesystem>

// User-defined input:
cv::String path{ "C:/Users/baghd/Videos/key0_mM_benzaldehyde_010.avi" }; //video file path
float rescaleFactor{ 1 }; // choose a value less than one to resize the image for viewing
bool saveTrackingResults{true}; // boolean to toggle saving the object detection data to a .csv in the ~ folder (powershell> $HOME).

double totalFramesInVideo{0};
std::ofstream outFile(path.substr(0, path.length() - 4) + "_blob-detection.csv"); //prepare output file
cv::Mat image;
cv::Mat img;
cv::Mat imgBinary;
int pressedKey{-1};

int main()
{
  std::cout << "Reading file: " << path << "\n" << "Press \"e\" or \"esc\" on active video window to exit playback." << std::endl;
  generateTrackbar(); // UpdateBrightnessAndContrast.h function (1of2) to allow for live, manual brightness and contrast control
  createControlsWindow(); // allow for showing multiple videos

  cv::VideoCapture cap(path);
  totalFramesInVideo = cap.get(cv::CAP_PROP_FRAME_COUNT);

  if (saveTrackingResults) // initialize tracking data output only if "saveTrackingResults" is set to "true"
  { 
    outFile << "frame,x_px,y_px,blob_size_px\n"; // include output file header line
  }
  while (true)
  {
    if (cap.get(cv::CAP_PROP_POS_FRAMES) == cap.get(cv::CAP_PROP_FRAME_COUNT)) break; //exit the playback loop when the video is over
    //std::cout << "Current frame (zero indexed)" << cap.get(cv::CAP_PROP_POS_FRAMES) << std::endl;
    //std::cout << "Next frame to be grabbed: " << cap.get(cv::CAP_PROP_POS_FRAMES) + 1 << std::endl;
    //std::cout << "Total number of frames: " << cap.get(cv::CAP_PROP_FRAME_COUNT) << std::endl;
    double currentFrame = cap.get(cv::CAP_PROP_POS_FRAMES); // query the current zero-indexed frame
    // double totalFramesInVideo = cap.get(cv::CAP_PROP_FRAME_COUNT); // 
    std::cout << "\r Frame(" << cap.get(cv::CAP_PROP_POS_FRAMES) << "/" << totalFramesInVideo;
    std::cout.flush();

    cap.read(image);
    cv::resize(image, image, cv::Size(image.size[1]*rescaleFactor, image.size[0]*rescaleFactor));
    cv::Mat new_image = updateBrightnessAndContrast(image); // UpdateBrightnessAndContrast.h function (1of2) to allow for live, manual brightness and contrast control
    cv::cvtColor(new_image, img, cv::COLOR_BGR2GRAY);
    cv::threshold(img, imgBinary, 250, 256, cv::THRESH_TRIANGLE);
    cv::bitwise_not(img, img);
    
    
    //Perform blob detection
    // 2. Setup SimpleBlobDetector parameters
    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 20;
    params.maxThreshold = 255;

    // Filter by Area.
    params.filterByArea = false;
    params.minArea = 5;
    params.maxArea = 100;

    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = 0.8;

    // Filter by Convexity
    params.filterByConvexity = false;
    params.minConvexity = 0.95;

    // Filter by Inertia
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;

    // 3. Create a SimpleBlobDetector object using the parameters
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

    // 4. Detect blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(img, keypoints);

    // Write keypoints to CSV
    if (saveTrackingResults)
    {
      for (const cv::KeyPoint& kp : keypoints) 
      {
        outFile << currentFrame << "," << kp.pt.x << "," << kp.pt.y << "," << kp.size << "\n";
      }
    }
    // 5. Draw detected blobs on the original image
    cv::Mat image_with_keypoints;
    cv::cvtColor(img, image_with_keypoints, cv::COLOR_GRAY2BGR); // Convert to color for drawing
    cv::drawKeypoints(image_with_keypoints, keypoints, image_with_keypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

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

    // GUI: Show windows
    if (updateShowIntermediateImages == true) // show wanted windows
    {
      cv::imshow("Original Image", image);
      cv::imshow("Adjusted B&C Image", new_image);
      cv::imshow("Grayscale Image", img);
      cv::imshow("Thresholded Image", imgBinary);
      
    }
    if (UpdateShowIntermediateImages == 0) // destroy unwanted windows
    {
      cv::destroyWindow("Original Image");
      cv::destroyWindow("Adjusted B&C Image");
      cv::destroyWindow("Grayscale Image");
      cv::destroyWindow("Thresholded Image");
    }
    cv::imshow("Blobs Detected", image_with_keypoints);

    pressedKey = cv::waitKey(1); //10 millisecond framerate
    if (pressedKey == 'e' || pressedKey == 27) break; //close if "e" or "escape" is pressed while a video window is active
    if (pressedKey == 'p') pressedKey = cv::waitKey(); // pause code execution if 'p' is 
  }
  std::cout<< "Next zero-indexed frame: " << cap.get(cv::CAP_PROP_POS_FRAMES); 
  if (outFile.is_open()) outFile.close();
  if (cap.isOpened()) cap.release();
  return 0;
}
