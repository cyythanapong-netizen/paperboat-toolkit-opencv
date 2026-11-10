// Flip: geometry of an image.

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

cv::Mat flip(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const int mode = option(q, "mode") == 1 ? 0 : option(q, "mode") == 2 ? -1 : 1;
  cv::flip(image, out, mode);
  return out;
}

}  // namespace

void add_flip(Registry& registry) {
  add_filter(registry, "flip", "Flip",
             {{.id = "mode", .type = "enum", .default_text = "horizontal", .options = {"horizontal", "vertical", "both"}}},
             flip, "Mirrors an image horizontally, vertically or both.");
}

}  // namespace paperboat::toolkit::opencv
