#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "UpdateBrightnessAndContrast.h" 
#include <opencv2/features2d.hpp>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include "BlobDetection.h"
#include "CalculateMoments.h" //need to clean up this file
#include "ReadCSV.h"
#include <vector>
#include "CalculateMSD.h"
#include "linkTrajectories.h"
#include "timeExecution.h" 
// Timing Notes:
// 37.7124 s @4:11pm, 
// 29.3756 s @ 4:42 pm (with others, changed waitKey(1) to pollKey), 
// 28.9186 s @ 4:59pm
// 27.7473 s if showVideo is false else 29.2881 s @ 5:06 pm

//TODO: pass to user input to console commands: user passes folder of uncompressed AVIs from ImageJ
// User-defined input TODO:
cv::String path{ "C:/Users/baghd/Videos/key0_mM_benzaldehyde_010.avi" };
float rescaleFactor{0.5}; // Choose value <= 1. Tradeoff between sensitivity and speed
bool setBrightnessAndContrastManually{false}; //toggle manual brightness and contrast control
bool showIntermediateImages{false}; // toggle display of intermediate processing steps
bool showVideo{true}; // toggle video and tracking playback
double totalFramesInVideo{0}; //object to store the video length in frames
cv::Mat image;
int pressedKey{ -1 }; //object to manage user input

int main()
{
  // timing code
  Timer t;
  std::cout << "Reading file: " << path << "\n" 
    << "Press \"e\" or \"esc\" on active video window to exit playback.\n"
    "Press \"p\" to pause execution.\n";

  if (setBrightnessAndContrastManually)
  {
    generateBrightnessAndContrastTrackbar();
  }

  cv::VideoCapture cap(path);
  totalFramesInVideo = cap.get(cv::CAP_PROP_FRAME_COUNT);

  std::ofstream outFile(path.substr(0, path.length() - 4) + "_blob-detection.csv"); //prepare output file
  outFile << "frame,x_px,y_px,blob_size_px\n"; // include output file header line

  while (true)
  {
    if (cap.get(cv::CAP_PROP_POS_FRAMES) == cap.get(cv::CAP_PROP_FRAME_COUNT)) break; //exit the playback loop when the video is over
    //double currentFrame = cap.get(cv::CAP_PROP_POS_FRAMES); // query the current zero-indexed frame
    std::cout << "\rFrame(" << cap.get(cv::CAP_PROP_POS_FRAMES) + 1 << "/" << totalFramesInVideo << ")";
    std::cout.flush();

    cap.read(image);
    cv::resize(image, image, cv::Size((int)((float)image.size[1] * rescaleFactor), (int)((float)image.size[0] * rescaleFactor)));
    image = updateBrightnessAndContrast2(image);
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    cv::bitwise_not(image, image);
    //object keypoints contains localization data for one frame
    std::vector<cv::KeyPoint> keypoints = DetectBlobs(cap, image);
    //calculateMoments(img, keypoints); TODO - update to fit gaussians

    // Write keypoints to CSV
    for (const cv::KeyPoint& kp : keypoints) 
    { 
      outFile << cap.get(cv::CAP_PROP_POS_FRAMES) << "," 
        << kp.pt.x << "," 
        << kp.pt.y << "," 
        << kp.size << "\n";
    }
    if (showVideo)
    {
      updateImageWithDetectedObjects(image, keypoints);
    }
    // manage user input while video is playing
    pressedKey = cv::pollKey();
    if (pressedKey == 'e' || pressedKey == 27) break; //close if "e" or "escape"
    if (pressedKey == 'p') pressedKey = cv::waitKey(); // pause code execution if 'p' 
  }
  std::cout << "\nCompleted processing " << cap.get(cv::CAP_PROP_POS_FRAMES) << " frames\n"; //upon completion, this should be the total number of captured frames
  if (outFile.is_open()) 
  {
    outFile.close();
  }
  if (cap.isOpened()) 
  {
    cap.release();
  }

  // read localizations from saved csv file
  std::vector<Localization> locs{}; //array of unlinked localizations
  locs = readCSV(path.substr(0, path.length() - 4) + "_blob-detection.csv");
  std::cout << locs.size() << " localizations successfully detected.\n";

  std::vector<std::vector<Localization>> trajectories; //array of trajectories
  trajectories = linkTrajectories(locs, path);

  calculateMSDsAndWriteMSDsToCSV(trajectories, path);
  // timing code
  std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

  return 0;
}