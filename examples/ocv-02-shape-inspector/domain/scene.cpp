// A deterministic test card: one filled circle and one filled rectangle on a dark field.

#include "scene.hpp"

#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace shape_inspector {
namespace {

class TestCard final : public paperboat::Block {
 public:
  paperboat::Values step(const paperboat::Context& ctx, const paperboat::Values&) override {
    cv::Mat image(240, 320, CV_8UC3, cv::Scalar(36, 36, 40));
    cv::circle(image, {90, 120}, 30, cv::Scalar(232, 232, 232), cv::FILLED, cv::LINE_AA);
    cv::rectangle(image, {210, 80, 60, 50}, cv::Scalar(232, 232, 232), cv::FILLED);
    ctx.log("test card: 1 circle, 2 shapes");
    return {{"image", paperboat::toolkit::opencv::image_out(std::move(image))}, {"circles", 1.0}, {"shapes", 2.0}};
  }
};

}  // namespace

void register_scene(paperboat::Registry& registry) {
  registry.add(paperboat::define_block<TestCard>(
      "shape-inspector",
      {.type = "testcard",
       .label = "Test card",
       .kind = paperboat::Kind::Source,
       .inputs = {},
       .outputs = {{"image", "image"}, {"circles"}, {"shapes"}},
       .params = {},
       .description = "A synthetic card with one circle and one rectangle."}));
}

}  // namespace shape_inspector
