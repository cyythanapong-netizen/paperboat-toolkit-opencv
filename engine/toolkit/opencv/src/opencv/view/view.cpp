// View: shows its input image on the Dashboard as an image instrument.

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

// Shows its input image on the Dashboard: an image instrument.
struct View final : Block {
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image) return {};
    return {{"value", data_url(*image)}};
  }
};

}  // namespace

void add_view(Registry& registry) {
  Template view;
  view.name = "opencv/view";
  view.def = {.type = "view", .label = "Image view", .kind = Kind::Monitor, .inputs = {image_port("image")},
              .outputs = {}, .params = {}, .instrument = "image", .description = "Shows an image on the Dashboard."};
  view.make = [](const Values&, const Config&) { return std::make_unique<View>(); };
  registry.add(std::move(view));
}

}  // namespace paperboat::toolkit::opencv
