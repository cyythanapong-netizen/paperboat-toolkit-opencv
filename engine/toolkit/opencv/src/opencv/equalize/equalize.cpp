// Equalize: colour and tone.

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

cv::Mat equalize(const cv::Mat& image, const Values&) {
  cv::Mat out;
  cv::equalizeHist(as_gray(image), out);
  return out;
}

}  // namespace

void add_equalize(Registry& registry) {
  add_filter(registry, "equalize", "Equalize", {}, equalize,
             "Spreads the histogram: more contrast in a flat or dark image.");
}

}  // namespace paperboat::toolkit::opencv
