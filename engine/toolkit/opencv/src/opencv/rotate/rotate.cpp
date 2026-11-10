// Rotate: geometry of an image.

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

cv::Mat rotate(const cv::Mat& image, const Values& q) {
  const double angle = p(q, "angle");
  const double scale = p(q, "scale") > 0.0 ? p(q, "scale") : 1.0;
  const cv::Point2f center((static_cast<float>(image.cols) - 1.0f) / 2.0f, (static_cast<float>(image.rows) - 1.0f) / 2.0f);
  cv::Mat matrix = cv::getRotationMatrix2D(center, angle, scale);
  cv::Size size(image.cols, image.rows);
  if (p(q, "expand") > 0.0) {
    const auto apply = [&](double x, double y) {
      return cv::Point2d(matrix.at<double>(0, 0) * x + matrix.at<double>(0, 1) * y + matrix.at<double>(0, 2),
                         matrix.at<double>(1, 0) * x + matrix.at<double>(1, 1) * y + matrix.at<double>(1, 2));
    };
    const cv::Point2d corners[] = {apply(0, 0), apply(image.cols, 0), apply(image.cols, image.rows),
                                   apply(0, image.rows)};
    double min_x = corners[0].x, max_x = corners[0].x, min_y = corners[0].y, max_y = corners[0].y;
    for (const auto& c : corners) {
      min_x = std::min(min_x, c.x);
      max_x = std::max(max_x, c.x);
      min_y = std::min(min_y, c.y);
      max_y = std::max(max_y, c.y);
    }
    matrix.at<double>(0, 2) -= min_x;
    matrix.at<double>(1, 2) -= min_y;
    size = {std::max(1, static_cast<int>(std::lround(max_x - min_x))),
            std::max(1, static_cast<int>(std::lround(max_y - min_y)))};
  }
  cv::Mat out;
  cv::warpAffine(image, out, matrix, size, cv::INTER_LINEAR, cv::BORDER_REPLICATE);
  return out;
}

}  // namespace

void add_rotate(Registry& registry) {
  add_filter(registry, "rotate", "Rotate", {{"angle", 0.0}, {"scale", 1.0}, {"expand", 0.0}}, rotate,
             "Rotates an image about its centre (expand = 1 keeps the corners in view).");
}

}  // namespace paperboat::toolkit::opencv
