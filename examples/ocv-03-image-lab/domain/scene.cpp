// A deterministic gradient card: a horizontal ramp with a bright square, and its grey
// statistics for the readouts to compare against.

#include "scene.hpp"

#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace image_lab {
namespace {

class TestCard final : public paperboat::Block {
 public:
  paperboat::Values step(const paperboat::Context& ctx, const paperboat::Values&) override {
    cv::Mat image(240, 320, CV_8UC3);
    for (int x = 0; x < image.cols; ++x) {
      const auto value = static_cast<uchar>(x * 255 / (image.cols - 1));
      image.col(x).setTo(cv::Scalar(value, value, value));
    }
    cv::rectangle(image, {230, 40, 40, 40}, cv::Scalar(255, 255, 255), cv::FILLED);

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    double min = 0.0;
    double max = 0.0;
    cv::minMaxLoc(gray, &min, &max);
    const double mean = cv::mean(gray)[0];
    ctx.log("image lab: gradient card");
    return {{"image", paperboat::toolkit::opencv::image_out(std::move(image))},
            {"mean", mean},
            {"min", min},
            {"max", max}};
  }
};

}  // namespace

void register_scene(paperboat::Registry& registry) {
  registry.add(paperboat::define_block<TestCard>(
      "image-lab",
      {.type = "testcard",
       .label = "Gradient card",
       .kind = paperboat::Kind::Source,
       .inputs = {},
       .outputs = {{"image", "image"}, {"mean"}, {"min"}, {"max"}},
       .params = {},
       .description = "A synthetic horizontal gradient with a bright square."}));
}

}  // namespace image_lab
