// Threshold: splits an image into black and white at a level.

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
void add_threshold(Registry& registry) {
  // Gray → black and white at `level` (0–255). otsu = 1 picks the level from the picture;
  // invert = 1 makes dark things white (parts on a light background).
  add_filter(registry, "threshold", "Threshold", {{"level", 128.0}, {"otsu", 1.0}, {"invert", 0.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               const int type = (p(q, "invert") > 0 ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY) |
                                (p(q, "otsu") > 0 ? cv::THRESH_OTSU : 0);
               cv::threshold(as_gray(im), out, p(q, "level"), 255, type);
               return out;
             },
             "Splits an image into black and white at a level.");
}

}  // namespace paperboat::toolkit::opencv
