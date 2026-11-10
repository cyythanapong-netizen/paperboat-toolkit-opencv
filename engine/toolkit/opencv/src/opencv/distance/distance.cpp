// Distance: how far each white pixel is from black.

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {
void add_distance(Registry& registry) {
  // How far each white pixel is from black: touching blobs get one peak each.
  add_filter(registry, "distance", "Distance", {},
             [](const cv::Mat& im, const Values&) {
               cv::Mat dist, out;
               cv::distanceTransform(as_gray(im), dist, cv::DIST_L2, 5);
               cv::normalize(dist, out, 0, 255, cv::NORM_MINMAX, CV_8U);
               return out;
             },
             "How far each white pixel is from black.");
}

}  // namespace paperboat::toolkit::opencv
