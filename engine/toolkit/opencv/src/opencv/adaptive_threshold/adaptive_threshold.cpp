// Adaptive threshold: black and white by local brightness.

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
void add_adaptive_threshold(Registry& registry) {
  // Threshold against the local mean: for uneven lighting.
  add_filter(registry, "adaptive_threshold", "Threshold (adaptive)", {{"size", 31.0}, {"c", 5.0}, {"invert", 0.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::adaptiveThreshold(as_gray(im), out, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                                     p(q, "invert") > 0 ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY,
                                     std::max(3, odd(p(q, "size"))), p(q, "c"));
               return out;
             },
             "Black and white by local brightness: for uneven light.");
}

}  // namespace paperboat::toolkit::opencv
