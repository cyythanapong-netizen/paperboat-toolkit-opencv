// Laplacian: gradient edges as an 8-bit image.

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

cv::Mat laplacian(const cv::Mat& image, const Values& q) {
  cv::Mat gradient;
  cv::Laplacian(as_gray(image), gradient, CV_16S, odd(p(q, "size")));
  cv::Mat out;
  cv::convertScaleAbs(gradient, out);
  return out;
}

}  // namespace

void add_laplacian(Registry& registry) {
  add_filter(registry, "laplacian", "Laplacian", {{"size", 3.0}}, laplacian,
             "The second derivative: edges in every direction.");
}

}  // namespace paperboat::toolkit::opencv
