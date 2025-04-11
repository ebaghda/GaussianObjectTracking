#pragma once
#include "ReadCSV.h"
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>

//using Trajectory = std::vector<Localization>;


float calculateDistance(const Localization& a, const Localization& b) {
  float dx = a.x_px - b.x_px;
  float dy = a.y_px - b.y_px;
  return std::sqrt(dx * dx + dy * dy);
}
// Write trajectories to CSV
void writeTrajectoriesToCSV(const std::vector<std::vector<Localization>>& trajectories, const std::string& path) {
  std::string filename { path.substr(0, path.length() - 4) + "_trajectories.csv" };
  std::ofstream outFile(filename);

  if (!outFile.is_open()) {
    std::cerr << "Error: could not open file " << filename << " for writing." << std::endl;
    return;
  }

  // Write header
  outFile << "trajectory_id,frame,x,y\n";

  for (size_t trajId = 0; trajId < trajectories.size(); ++trajId) {
    const auto& traj = trajectories[trajId];
    for (const auto& loc : traj) {
      outFile << trajId << "," << loc.frame << "," << loc.x_px << "," << loc.y_px << "\n";
    }
  }

  outFile.close();
  std::cout << "Trajectories written to " << filename << "\n";
}

std::vector<std::vector<Localization>> linkTrajectories(std::vector<Localization> locs, std::string& path)
{
  // perform trajectory linking 
  float distanceThreshold{ 10 }; // define the maximum search distance in pixels 
  std::vector<std::vector<Localization>> trajectories;

  for (int i = 0; i < locs.size(); ++i)
  {
    Localization& currentLoc = locs[i];
    bool linked = false;

    // Attempt to link to existing trajectories
    for (std::vector<Localization>& traj : trajectories)
    {
      Localization& lastLoc = traj.back();
      float distance = calculateDistance(currentLoc, lastLoc); // Implement your distance calculation function

      if (distance <= distanceThreshold)
      {
        traj.push_back(currentLoc);
        linked = true;
        break; // Linking successful, exit loop
      }
    }

    // If not linked, start a new trajectory
    if (!linked)
    {
      trajectories.push_back({ currentLoc });
    }
  }

  // Now trajectories vector contains linked trajectories
  // Process trajectories further as needed
  writeTrajectoriesToCSV(trajectories, path);
  return trajectories;
}