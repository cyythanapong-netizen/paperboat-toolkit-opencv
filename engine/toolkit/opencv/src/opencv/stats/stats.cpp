// stats: image analysis.

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

class Stats final : public Block {
 public:
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    cv::Scalar mean;
    cv::Scalar stddev;
    cv::meanStdDev(gray, mean, stddev);
    double min = 0.0;
    double max = 0.0;
    cv::minMaxLoc(gray, &min, &max);
    return {{"mean", mean[0]},
            {"std", stddev[0]},
            {"min", min},
            {"max", max},
            {"nonzero", static_cast<double>(cv::countNonZero(gray))},
            {"pixels", static_cast<double>(gray.total())}};
  }
};

}  // namespace

void add_stats(Registry& registry) {
  Template t;
  t.name = "opencv/stats";
  t.def = {.type = "stats", .label = "Image stats", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {{"mean"}, {"std"}, {"min"}, {"max"}, {"nonzero"}, {"pixels"}},
           .params = {}, .description = "Mean, spread, range and non-black count of an image.", .cacheable = true};
  t.make = [](const Values&, const Config&) { return std::make_unique<Stats>(); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
