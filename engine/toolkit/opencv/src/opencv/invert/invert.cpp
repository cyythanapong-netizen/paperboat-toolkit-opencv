// Invert: swaps black and white.

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
void add_invert(Registry& registry) {
  add_filter(registry, "invert", "Invert", {},
             [](const cv::Mat& im, const Values&) {
               cv::Mat out;
               cv::bitwise_not(im, out);
               return out;
             },
             "Swaps black and white.");
}

}  // namespace paperboat::toolkit::opencv
