// Geometry: resize, crop, flip and rotate an image.

#include <algorithm>
#include <cmath>
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

cv::Mat flip(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const int mode = option(q, "mode") == 1 ? 0 : option(q, "mode") == 2 ? -1 : 1;
  cv::flip(image, out, mode);
  return out;
}

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

void add_geometry(Registry& registry) {
  add_filter(registry, "resize", "Resize",
             {{"width", 0.0},
              {"height", 0.0},
              {"scale", 1.0},
              {.id = "interpolation",
               .type = "enum",
               .default_text = "linear",
               .options = {"linear", "nearest", "cubic", "area"}}},
             resize, "Changes an image's size (pixels, or a scale when both are 0).");

  add_filter(registry, "crop", "Crop", {{"x", 0.0}, {"y", 0.0}, {"width", 0.0}, {"height", 0.0}}, crop,
             "Keeps a rectangle of an image (0 width or height: the rest of it).");

  add_filter(registry, "flip", "Flip",
             {{.id = "mode", .type = "enum", .default_text = "horizontal", .options = {"horizontal", "vertical", "both"}}},
             flip, "Mirrors an image horizontally, vertically or both.");

  add_filter(registry, "rotate", "Rotate", {{"angle", 0.0}, {"scale", 1.0}, {"expand", 0.0}}, rotate,
             "Rotates an image about its centre (expand = 1 keeps the corners in view).");
}

}  // namespace paperboat::toolkit::opencv
