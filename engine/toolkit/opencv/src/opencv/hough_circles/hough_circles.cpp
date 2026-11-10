// hough_circles: image analysis.

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

class HoughCircles final : public Block {
 public:
  explicit HoughCircles(const Values& q)
      : dp_(p(q, "dp") > 0.0 ? p(q, "dp") : 1.2),
        min_dist_(p(q, "min_dist") > 0.0 ? p(q, "min_dist") : 20.0),
        param1_(std::max(1.0, p(q, "param1"))),
        param2_(std::max(1.0, p(q, "param2"))),
        min_radius_(std::max(0.0, p(q, "min_radius"))),
        max_radius_(std::max(0.0, p(q, "max_radius"))),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, dp_, min_dist_, param1_, param2_,
                     static_cast<int>(std::lround(min_radius_)), static_cast<int>(std::lround(max_radius_)));

    cv::Mat marked = as_bgr(gray).clone();
    double radius = 0.0;
    double u = 0.0;
    double v = 0.0;
    for (const auto& circle : circles) {
      if (circle[2] > radius) {
        radius = circle[2];
        u = circle[0];
        v = circle[1];
      }
      cv::circle(marked, {static_cast<int>(std::lround(circle[0])), static_cast<int>(std::lround(circle[1]))},
                 static_cast<int>(std::lround(circle[2])), cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
      cv::circle(marked, {static_cast<int>(std::lround(circle[0])), static_cast<int>(std::lround(circle[1]))}, 2,
                 cv::Scalar(255, 160, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(circles.size())},
            {"u", u},
            {"v", v},
            {"radius", radius}};
  }

 private:
  double dp_;
  double min_dist_;
  double param1_;
  double param2_;
  double min_radius_;
  double max_radius_;
  int thickness_;
};

}  // namespace

void add_hough_circles(Registry& registry) {
  Template t;
  t.name = "opencv/hough_circles";
  t.def = {.type = "hough_circles", .label = "Hough circles", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"u"}, {"v"}, {"radius"}},
           .params = {{"dp", 1.2},       {"min_dist", 20.0}, {"param1", 100.0},
                      {"param2", 30.0},  {"min_radius", 0.0}, {"max_radius", 0.0},
                      {"thickness", 1.0}},
           .description = "Finds circles: how many, the largest one's centre and radius, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<HoughCircles>(q); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
