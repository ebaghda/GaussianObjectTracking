#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "UpdateBrightnessAndContrast.h" // header file to allow for live, manual brightness and contrast control
#include <opencv2/features2d.hpp>
//#include "GUIControls.h" //need to clean up this file
#include <iomanip>
#include <fstream>
#include <filesystem>
#include "BlobDetection.h"
#include "CalculateMoments.h" //need to clean up this file

// User-defined input:
cv::String path{ "C:/Users/baghd/Videos/key0_mM_benzaldehyde_010.avi" }; //video file path
float rescaleFactor{ 0.5 }; // image resizing upon load. Choose 1 for maximum sensitivity choose a value less than one to resize the image for viewing and for speed
bool saveTrackingResults{true}; // boolean to toggle saving the object detection data to a .csv in the ~ folder (powershell> $HOME).
bool showIntermediateImages{true}; // bolean to toggle display of intermediate processing steps

double totalFramesInVideo{0};
std::ofstream outFile(path.substr(0, path.length() - 4) + "_blob-detection.csv"); //prepare output file
cv::Mat image;
cv::Mat img;
int pressedKey{-1}; //initiallize pressedKeyvalue

///////////////////////////////////////////////////          MAIN          ///////////////////////////////////////////////////
int main()
{
  std::cout << "Reading file: " << path << "\n" << "Press \"e\" or \"esc\" on active video window to exit playback." << std::endl;
  generateBrightnessAndContrastTrackbar(); // UpdateBrightnessAndContrast.h function (1of2) to allow for live, manual brightness and contrast control

  cv::VideoCapture cap(path);
  totalFramesInVideo = cap.get(cv::CAP_PROP_FRAME_COUNT);

  if (saveTrackingResults) // initialize tracking data output only if "saveTrackingResults" is set to "true"
    outFile << "frame,x_px,y_px,blob_size_px\n"; // include output file header line

  while (true)
  {
    if (cap.get(cv::CAP_PROP_POS_FRAMES) == cap.get(cv::CAP_PROP_FRAME_COUNT)) break; //exit the playback loop when the video is over
    double currentFrame = cap.get(cv::CAP_PROP_POS_FRAMES); // query the current zero-indexed frame
    std::cout << "\r Frame(" << cap.get(cv::CAP_PROP_POS_FRAMES) << "/" << totalFramesInVideo;
    std::cout.flush();

    cap.read(image);
    cv::resize(image, image, cv::Size(image.size[1]*rescaleFactor, image.size[0]*rescaleFactor));
    cv::Mat new_image = updateBrightnessAndContrast(image); // UpdateBrightnessAndContrast.h function (1of2) to allow for live, manual brightness and contrast control
    cv::cvtColor(new_image, img, cv::COLOR_BGR2GRAY);
    cv::bitwise_not(img, img);
     
    std::vector<cv::KeyPoint> keypoints = DetectBlobs(cap, img);
    //calculateMoments(img, keypoints); TODO - update to fit gaussians
    
    // Write keypoints to CSV
    if (saveTrackingResults){
      for (const cv::KeyPoint& kp : keypoints) outFile << cap.get(cv::CAP_PROP_POS_FRAMES) << "," << kp.pt.x << "," << kp.pt.y << "," << kp.size << "\n";
    }
    // Draw detected blobs on the original image
    updateImageWithDetectedObjects(img, keypoints);

    // GUI: Show windows
    if (showIntermediateImages) // show wanted windows
    {
      cv::imshow("Original Image", image);
      cv::imshow("Adjusted B&C Image", new_image);
      cv::imshow("Grayscale Image", img);
      
    }

    // Manage user input while video is playing
    pressedKey = cv::waitKey(1); //10 millisecond framerate
    if (pressedKey == 'e' || pressedKey == 27) break; //close if "e" or "escape" is pressed while a video window is active
    if (pressedKey == 'p') pressedKey = cv::waitKey(); // pause code execution if 'p' is 
  }
  std::cout<< "\nCompleted processing " << cap.get(cv::CAP_PROP_POS_FRAMES) << "Frames\n" << std::endl; //upon completion, this should be the total number of captured frames
  if (outFile.is_open()) outFile.close(); //file cleanup
  if (cap.isOpened()) cap.release(); //file cleanup
  return 0;
}
