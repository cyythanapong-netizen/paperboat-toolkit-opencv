// contours: image analysis.

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

// The largest contour of a set, or null.
const std::vector<cv::Point>* largest(const std::vector<const std::vector<cv::Point>*>& contours) {
  const std::vector<cv::Point>* best = nullptr;
  double best_area = -1.0;
  for (const auto* contour : contours) {
    const double area = cv::contourArea(*contour);
    if (area > best_area) {
      best_area = area;
      best = contour;
    }
  }
  return best;
}

class Contours final : public Block {
 public:
  explicit Contours(const Values& q)
      : min_area_(p(q, "min_area")), max_area_(p(q, "max_area")), draw_(option(q, "draw")),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<std::vector<cv::Point>> all;
    cv::findContours(gray.clone(), all, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<const std::vector<cv::Point>*> kept;
    for (const auto& contour : all) {
      const double area = cv::contourArea(contour);
      if (area < min_area_ || (max_area_ > 0.0 && area > max_area_)) continue;
      kept.push_back(&contour);
    }

    cv::Mat marked = as_bgr(gray).clone();
    if (draw_ == 0) {
      for (const auto* contour : kept) cv::drawContours(marked, std::vector<std::vector<cv::Point>>{*contour}, -1, cv::Scalar(120, 255, 120),
                                                        thickness_, cv::LINE_AA);
    }

    const std::vector<cv::Point>* best = largest(kept);
    if (!best) {
      return {{"image", image_out(std::move(marked))}, {"count", static_cast<double>(kept.size())},
              {"u", 0.0}, {"v", 0.0}, {"area", 0.0}, {"width", 0.0}, {"height", 0.0}, {"found", 0.0}};
    }

    const double area = cv::contourArea(*best);
    const cv::Moments moments = cv::moments(*best, false);
    const double u = moments.m00 > 0.0 ? moments.m10 / moments.m00 : 0.0;
    const double v = moments.m00 > 0.0 ? moments.m01 / moments.m00 : 0.0;
    const cv::Rect box = cv::boundingRect(*best);
    if (draw_ != 2) {
      cv::rectangle(marked, box, cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
      cv::circle(marked, {static_cast<int>(std::lround(u)), static_cast<int>(std::lround(v))}, 4,
                 cv::Scalar(255, 160, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(kept.size())},
            {"u", u},
            {"v", v},
            {"area", area},
            {"width", static_cast<double>(box.width)},
            {"height", static_cast<double>(box.height)},
            {"found", 1.0}};
  }

 private:
  double min_area_;
  double max_area_;
  int draw_;  // 0 all, 1 largest, 2 none
  int thickness_;
};

}  // namespace

void add_contours(Registry& registry) {
  Template t;
  t.name = "opencv/contours";
  t.def = {.type = "contours", .label = "Contours", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"u"}, {"v"}, {"area"}, {"width"}, {"height"}, {"found"}},
           .params = {{"min_area", 0.0},
                      {"max_area", 0.0},
                      {.id = "draw", .type = "enum", .default_text = "all", .options = {"all", "largest", "none"}},
                      {"thickness", 1.0}},
           .description = "The white blobs' count, the largest one's centre, area and box, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<Contours>(q); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
