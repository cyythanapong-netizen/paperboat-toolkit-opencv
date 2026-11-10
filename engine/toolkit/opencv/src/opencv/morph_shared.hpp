#pragma once

// Shared by erode, dilate, open and close: the same filter with a different morphological op.

#include <algorithm>
#include <string>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {

inline cv::Mat morph(const cv::Mat& image, const Values& q, cv::MorphTypes op) {
  const int k = odd(p(q, "size"));
  cv::Mat out;
  cv::morphologyEx(image, out, op, cv::getStructuringElement(cv::MORPH_ELLIPSE, {k, k}), {-1, -1},
                   std::max(1, static_cast<int>(p(q, "iterations"))));
  return out;
}

inline void add_morph(Registry& registry, const std::string& type, std::string label, cv::MorphTypes op,
                      std::string description) {
  add_filter(registry, type, std::move(label), {{"size", 5.0}, {"iterations", 1.0}},
             [op](const cv::Mat& im, const Values& q) { return morph(im, q, op); }, std::move(description));
}

}  // namespace paperboat::toolkit::opencv
