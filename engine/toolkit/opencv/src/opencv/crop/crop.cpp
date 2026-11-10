// Crop: geometry of an image.

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

cv::Mat crop(const cv::Mat& image, const Values& q) {
  const int x = std::clamp(static_cast<int>(std::lround(p(q, "x"))), 0, std::max(0, image.cols - 1));
  const int y = std::clamp(static_cast<int>(std::lround(p(q, "y"))), 0, std::max(0, image.rows - 1));
  int width = static_cast<int>(std::lround(p(q, "width")));
  int height = static_cast<int>(std::lround(p(q, "height")));
  width = width <= 0 ? image.cols - x : std::min(width, image.cols - x);
  height = height <= 0 ? image.rows - y : std::min(height, image.rows - y);
  if (width <= 0 || height <= 0) return {};
  return image(cv::Rect(x, y, width, height)).clone();
}

}  // namespace

void add_crop(Registry& registry) {
  add_filter(registry, "crop", "Crop", {{"x", 0.0}, {"y", 0.0}, {"width", 0.0}, {"height", 0.0}}, crop,
             "Keeps a rectangle of an image (0 width or height: the rest of it).");
}

}  // namespace paperboat::toolkit::opencv
