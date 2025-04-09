#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


struct Localization
{
  int frame;
  float x_px;
  float y_px;
  float size;
};

std::vector<Localization> readCSV(std::string filename)
{
  std::vector<Localization> locs;
  std::ifstream file(filename);
  std::puts("Reading CSV");

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return locs;
  }
  std::string line;
  std::getline(file, line);

  while (std::getline(file, line))
  {
    std::stringstream ss(line);
    Localization point;
    std::string frameStr, xStr, yStr, sizeStr;

    if (std::getline(ss, frameStr, ',') &&
      std::getline(ss, xStr, ',') &&
      std::getline(ss, yStr, ',') &&
      std::getline(ss, sizeStr, ',')) 
    {
      point.frame = std::stoi(frameStr);
      point.x_px = std::stof(xStr);
      point.y_px = std::stof(yStr);
      point.size = std::stof(sizeStr);
      locs.push_back(point);
    }
  }
  file.close();
  std::puts("Completed reading CSV");
}
