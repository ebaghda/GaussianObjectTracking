#pragma once
#include <iostream>
#include <opencv2/highgui.hpp>

bool updateShowIntermediateImages{false};

void UpdateShowIntermediateImages(int pos, void* userdata)
{
  (void) userdata;
  if (pos == 1)
  {
    updateShowIntermediateImages = true;
    std::cout << "Updated intermediate image visibility." << std::endl;
  }
  else
  {
  updateShowIntermediateImages = false;
  }
}

void createControlsWindow() 
{
  cv::namedWindow("Controls");
  cv::resizeWindow("Controls", cv::Size(640, 190));
  cv::createTrackbar("Show Intermediate Images", "Controls", NULL, 1, UpdateShowIntermediateImages);


}
