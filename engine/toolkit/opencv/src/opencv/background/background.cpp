// Background: the pixels that changed since the learned background.

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

class Background final : public Block {
 public:
  explicit Background(const Values& q)
      : rate_(std::clamp(p(q, "rate"), 0.0, 1.0)),
        threshold_(std::max(1.0, p(q, "threshold"))),
        open_(p(q, "open") > 0.0) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));

    if (background_.empty()) {
      background_ = gray.clone();
      cv::Mat empty = cv::Mat::zeros(gray.size(), CV_8U);
      return {{"image", image_out(std::move(empty))}, {"background", image_out(background_.clone())}, {"changed", 0.0}};
    }
    if (background_.size() != gray.size()) throw std::runtime_error("background: the image size changed");

    cv::Mat diff;
    cv::absdiff(gray, background_, diff);
    cv::Mat mask;
    cv::threshold(diff, mask, threshold_, 255, cv::THRESH_BINARY);
    if (open_) {
      cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3}));
    }
    const double changed = static_cast<double>(cv::countNonZero(mask)) / static_cast<double>(mask.total());
    cv::Mat updated;
    cv::addWeighted(background_, 1.0 - rate_, gray, rate_, 0.0, updated);
    background_ = std::move(updated);
    return {{"image", image_out(std::move(mask))}, {"background", image_out(background_.clone())}, {"changed", changed}};
  }

 private:
  double rate_;
  double threshold_;
  bool open_;
  cv::Mat background_;
};

}  // namespace

void add_background(Registry& registry) {
  Template t;
  t.name = "opencv/background";
  t.def = {.type = "background", .label = "Background", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), image_port("background"), {"changed"}},
           .params = {{"rate", 0.02}, {"threshold", 20.0}, {"open", 1.0}},
           .description = "Learns a background and shows what changed: a motion mask, the background and the changed fraction.",
           .cacheable = false};
  t.make = [](const Values& q, const Config&) { return std::make_unique<Background>(q); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
