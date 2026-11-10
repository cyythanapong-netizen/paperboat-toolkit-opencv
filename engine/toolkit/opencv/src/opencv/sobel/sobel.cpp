// Sobel: gradient edges as an 8-bit image.

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
namespace {

cv::Mat sobel(const cv::Mat& image, const Values& q) {
  const int dx = std::clamp(static_cast<int>(std::lround(p(q, "dx"))), 0, 2);
  const int dy = std::clamp(static_cast<int>(std::lround(p(q, "dy"))), 0, 2);
  if (dx == 0 && dy == 0) return as_gray(image).clone();
  cv::Mat gradient;
  cv::Sobel(as_gray(image), gradient, CV_16S, dx, dy, odd(p(q, "size")));
  cv::Mat out;
  cv::convertScaleAbs(gradient, out);
  return out;
}

}  // namespace

void add_sobel(Registry& registry) {
  add_filter(registry, "sobel", "Sobel", {{"dx", 1.0}, {"dy", 0.0}, {"size", 3.0}}, sobel,
             "A directional derivative: bright where the image changes along x or y.");
}

}  // namespace paperboat::toolkit::opencv
