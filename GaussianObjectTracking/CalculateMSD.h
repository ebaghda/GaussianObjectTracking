#pragma once
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>


std::vector<float> computeMSD(const std::vector<Localization>& traj) {
  cv::uint64_t N = traj.size();
  std::vector<float> msd;

  if (N < 2) return msd;  // not enough points

  cv::uint64_t maxLag = N / 2;  // or another value depending on your preference

  for (int lag = 1; lag <= maxLag; ++lag) {
    float sum = 0.0f;
    int count = 0;

    for (long long i = 0; i + lag < N; ++i) {
      float dx = traj[i + lag].x_px - traj[i].x_px;
      float dy = traj[i + lag].y_px - traj[i].y_px;
      auto squaredDisplacement = dx * dx + dy * dy;

      sum += squaredDisplacement;
      ++count;
    }

    msd.push_back(count > 0 ? sum / count : 0.0f);
  }

  return msd;
}


void calculateMSDsAndWriteMSDsToCSV(const std::vector<std::vector<Localization>>& trajectories, const std::string& path) {
std::puts("Calculating MSDs...");
  std::string filename { path.substr(0, path.length() - 4) + "_msd.csv" };
  std::ofstream outFile(filename);

  if (!outFile.is_open()) {
    std::cerr << "Error: could not open file " << filename << " for writing MSDs." << std::endl;
    return;
  }

  // Determine the maximum number of lags from all trajectories
  size_t maxLags{0};
  for (const auto& traj : trajectories) {
    maxLags = std::max(maxLags, traj.size() / 2);
  }

  // Write header
  outFile << "trajectory_id";
  for (size_t lag = 1; lag <= maxLags; ++lag) {
    outFile << ",lag_" << lag;
  }
  outFile << "\n";

  // Write MSD values
  for (size_t trajId = 0; trajId < trajectories.size(); ++trajId) {
    auto msd = computeMSD(trajectories[trajId]);
    outFile << trajId;
    for (float value : msd) {
      outFile << "," << value;
    }
    outFile << "\n";
  }

  outFile.close();
  std::cout << "MSDs written to " << filename << std::endl;
}