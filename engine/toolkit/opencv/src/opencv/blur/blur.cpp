// Blur (Gaussian): softens an image.

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
void add_blur(Registry& registry) {
  add_filter(registry, "blur", "Blur (Gaussian)", {{"size", 5.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               const int k = odd(p(q, "size"));
               cv::GaussianBlur(im, out, {k, k}, 0);
               return out;
             },
             "Softens an image.");
}

}  // namespace paperboat::toolkit::opencv
