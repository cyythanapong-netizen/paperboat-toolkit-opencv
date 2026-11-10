// A deterministic overhead view of a conveyor with three bright parts and one speck of
// dust. The speck is small enough for the `open` stage to remove.

#include "scene.hpp"

#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace parts_counter {
namespace {

class Conveyor final : public paperboat::Block {
 public:
  paperboat::Values step(const paperboat::Context& ctx, const paperboat::Values&) override {
    cv::Mat image(240, 320, CV_8UC3, cv::Scalar(38, 38, 42));
    const cv::Rect parts[] = {{40, 80, 40, 30}, {140, 110, 50, 35}, {250, 70, 30, 25}};
    for (const auto& part : parts) cv::rectangle(image, part, cv::Scalar(232, 232, 232), cv::FILLED);
    cv::circle(image, {205, 200}, 2, cv::Scalar(232, 232, 232), cv::FILLED);
    ctx.log("conveyor: 3 parts, 1 speck");
    return {{"image", paperboat::toolkit::opencv::image_out(std::move(image))}, {"parts", 3.0}};
  }
};

}  // namespace

void register_scene(paperboat::Registry& registry) {
  registry.add(paperboat::define_block<Conveyor>(
      "parts-counter",
      {.type = "conveyor",
       .label = "Conveyor",
       .kind = paperboat::Kind::Source,
       .inputs = {},
       .outputs = {{"image", "image"}, {"parts"}},
       .params = {},
       .description = "A synthetic overhead conveyor with three parts and a speck of dust."}));
}

}  // namespace parts_counter
