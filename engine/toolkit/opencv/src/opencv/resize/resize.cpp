// Resize: geometry of an image.

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

int interpolation(const Values& q) {
  switch (option(q, "interpolation")) {
    case 1: return cv::INTER_NEAREST;
    case 2: return cv::INTER_CUBIC;
    case 3: return cv::INTER_AREA;
    default: return cv::INTER_LINEAR;
  }
}

cv::Mat resize(const cv::Mat& image, const Values& q) {
  int width = std::max(0, static_cast<int>(std::lround(p(q, "width"))));
  int height = std::max(0, static_cast<int>(std::lround(p(q, "height"))));
  const double scale = p(q, "scale");
  if (width <= 0 && height <= 0) {
    if (scale <= 0.0) return image.clone();
    width = std::max(1, static_cast<int>(std::lround(static_cast<double>(image.cols) * scale)));
    height = std::max(1, static_cast<int>(std::lround(static_cast<double>(image.rows) * scale)));
  } else if (width <= 0) {
    width = std::max(1, static_cast<int>(std::lround(static_cast<double>(image.cols) * height / image.rows)));
  } else if (height <= 0) {
    height = std::max(1, static_cast<int>(std::lround(static_cast<double>(image.rows) * width / image.cols)));
  }
  cv::Mat out;
  cv::resize(image, out, {width, height}, 0.0, 0.0, interpolation(q));
  return out;
}

}  // namespace

void add_resize(Registry& registry) {
  add_filter(registry, "resize", "Resize",
             {{"width", 0.0},
              {"height", 0.0},
              {"scale", 1.0},
              {.id = "interpolation",
               .type = "enum",
               .default_text = "linear",
               .options = {"linear", "nearest", "cubic", "area"}}},
             resize, "Changes an image's size (pixels, or a scale when both are 0).");
}

}  // namespace paperboat::toolkit::opencv
