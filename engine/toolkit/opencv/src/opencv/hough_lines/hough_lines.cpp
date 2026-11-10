// hough_lines: image analysis.

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

class HoughLines final : public Block {
 public:
  explicit HoughLines(const Values& q)
      : threshold_(std::max(1, static_cast<int>(std::lround(p(q, "threshold"))))),
        min_length_(std::max(0.0, p(q, "min_length"))),
        max_gap_(std::max(0.0, p(q, "max_gap"))),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(gray, lines, 1.0, CV_PI / 180.0, threshold_, min_length_, max_gap_);

    cv::Mat marked = as_bgr(gray).clone();
    double longest = 0.0;
    double angle = 0.0;
    for (const auto& line : lines) {
      const double dx = line[2] - line[0];
      const double dy = line[3] - line[1];
      const double length = std::hypot(dx, dy);
      if (length > longest) {
        longest = length;
        angle = std::atan2(dy, dx) * 180.0 / CV_PI;
      }
      cv::line(marked, {line[0], line[1]}, {line[2], line[3]}, cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(lines.size())},
            {"angle", angle},
            {"length", longest}};
  }

 private:
  int threshold_;
  double min_length_;
  double max_gap_;
  int thickness_;
};

}  // namespace

void add_hough_lines(Registry& registry) {
  Template t;
  t.name = "opencv/hough_lines";
  t.def = {.type = "hough_lines", .label = "Hough lines", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"angle"}, {"length"}},
           .params = {{"threshold", 80.0}, {"min_length", 30.0}, {"max_gap", 5.0}, {"thickness", 1.0}},
           .description = "Finds straight lines: how many, the longest one's angle and length, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<HoughLines>(q); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
