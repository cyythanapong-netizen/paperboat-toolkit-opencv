// Normalize: colour and tone.

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

int norm_type(const Values& q) {
  switch (option(q, "mode")) {
    case 1: return cv::NORM_L1;
    case 2: return cv::NORM_L2;
    case 3: return cv::NORM_INF;
    default: return cv::NORM_MINMAX;
  }
}

cv::Mat normalize(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const double alpha = p(q, "alpha");
  const double beta = p(q, "beta");
  cv::normalize(image, out, alpha, beta, norm_type(q), CV_8U);
  return out;
}

}  // namespace

void add_normalize(Registry& registry) {
  add_filter(registry, "normalize", "Normalize",
             {{"alpha", 0.0},
              {"beta", 255.0},
              {.id = "mode", .type = "enum", .default_text = "minmax", .options = {"minmax", "l1", "l2", "inf"}}},
             normalize, "Stretches an image's values into a range.");
}

}  // namespace paperboat::toolkit::opencv
