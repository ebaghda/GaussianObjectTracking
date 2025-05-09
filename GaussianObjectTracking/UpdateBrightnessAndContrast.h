/* Pseudo-code example for header function use: 
* // header file to allow for live, manual brightness and contrast control
* cv::Mat img;
* int main ()
* {
* generateBrightnessAndContrastTrackbar(); //UpdateBrightnessAndContrast.h function (1of2) to allow for live, manual brightness and contrast control 
* std::string path{"path/to/video.avi"};
* 
* cv::VideoCapture cap(path);
* while(true)
* {
*   cap.read(img);
*   image = updateBrightnessAndContrast(cv::Mat image) //UpdateBrightnessAndContrast.h function (2of2) to allow for live, manual brightness and contrast control 
*   cv::imshow("image", image);
*   waitkey(10);
}
* return 0;
* }
*/

#pragma once

#include <iostream>
#include <opencv2/highgui.hpp>

// variables for manual brightness and contrast selection
int alpha{ 12 };
int beta{ 0 };

void on_alpha_trackbar(int pos, void* userdata) 
{
  (void) userdata;
  alpha = pos;
  std::cout << "\nAlpha/gain/contrast set to: " << alpha << std::endl;
}

void on_beta_trackbar(int pos, void* userdata)
{
  (void) userdata;
  beta = pos;
  std::cout << "\nBeta/bias/brightness set to: " << beta << std::endl;
}

void generateBrightnessAndContrastTrackbar()
{
  cv::namedWindow("Trackbars", cv::WINDOW_NORMAL);
  cv::resizeWindow("Trackbars", 640, 190);
  cv::createTrackbar("Alpha", "Trackbars", NULL, 100, on_alpha_trackbar);
  on_alpha_trackbar(alpha, nullptr);
  cv::createTrackbar("Beta", "Trackbars", NULL, 100, on_beta_trackbar);
  on_beta_trackbar(beta, nullptr);

}

cv::Mat updateBrightnessAndContrast(cv::Mat image)
{
  cv::Mat new_image = cv::Mat::zeros(image.size(), image.type());
  /*the following is a depreciated for loop which performs the transformation pixel-by-pixel*/
  //for (int y = 0; y < image.rows; y++)
  //{
  //  for (int x = 0; x < image.cols; x++)
  //  {
  //    for (int c = 0; c < image.channels(); c++)
  //    {
  //      new_image.at<cv::Vec3b>(y, x)[c] = 
  //      cv::saturate_cast<uchar>(alpha * image.at<cv::Vec3b>(y, x)[c] + beta);
  //    }
  //  }
  //}
  image.convertTo(new_image, -1, alpha, beta); //replaces the for loop to perform a transformation output = alpha*input+beta
  return new_image;
}
cv::Mat updateBrightnessAndContrast2(cv::Mat image)
{
  image.convertTo(image, -1, alpha, beta); //replaces the for loop to perform a transformation output = alpha*input+beta
  return image;
}