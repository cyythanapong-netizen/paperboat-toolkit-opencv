// opencv/centroid: the centroid and apparent size of the largest white blob.
//
// A general stage at the end of a threshold/morphology pipeline: it finds the largest
// contour in a mask, returns its image-space centroid `u`,`v`, its area and equivalent
// radius, and a marked copy of the input for an Image view.

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {
namespace {

class Centroid final : public Block {
 public:
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return {{"image", image_out(gray.clone())}, {"u", 0.0}, {"v", 0.0},
                                  {"area", 0.0}, {"radius", 0.0}, {"found", 0.0}};

    const auto largest = std::max_element(
        contours.begin(), contours.end(),
        [](const auto& a, const auto& b) { return cv::contourArea(a) < cv::contourArea(b); });
    const double area = cv::contourArea(*largest);
    const cv::Moments moments = cv::moments(*largest, false);
    const double u = area > 0.0 ? moments.m10 / moments.m00 : 0.0;
    const double v = area > 0.0 ? moments.m01 / moments.m00 : 0.0;
    const double radius = std::sqrt(area / CV_PI);

    cv::Mat marked;
    if (gray.channels() == 1) cv::cvtColor(gray, marked, cv::COLOR_GRAY2BGR);
    else marked = gray.clone();
    cv::drawContours(marked, std::vector<std::vector<cv::Point>>{*largest}, -1, cv::Scalar(120, 255, 120), 1,
                     cv::LINE_AA);
    cv::circle(marked, {static_cast<int>(u), static_cast<int>(v)}, 8, cv::Scalar(120, 255, 120), 1, cv::LINE_AA);
    cv::line(marked, {static_cast<int>(u) - 14, static_cast<int>(v)},
             {static_cast<int>(u) + 14, static_cast<int>(v)}, cv::Scalar(120, 255, 120), 1, cv::LINE_AA);
    cv::line(marked, {static_cast<int>(u), static_cast<int>(v) - 14},
             {static_cast<int>(u), static_cast<int>(v) + 14}, cv::Scalar(120, 255, 120), 1, cv::LINE_AA);
    return {{"image", image_out(std::move(marked))}, {"u", u}, {"v", v}, {"area", area},
            {"radius", radius}, {"found", area > 0.0 ? 1.0 : 0.0}};
  }
};

}  // namespace

void add_centroid(Registry& registry) {
  Template t;
  t.name = "opencv/centroid";
  t.def = {.type = "centroid",
           .label = "Centroid",
           .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"u"}, {"v"}, {"area"}, {"radius"}, {"found"}},
           .params = {},
           .description = "The largest white blob's centroid, area and radius, plus a marked image.",
           .cacheable = true};
  t.make = [](const Values&, const Config&) { return std::make_unique<Centroid>(); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
