// Annotation: draw a box, circle, cross or text on an image.

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {
namespace {

cv::Scalar colour(int choice) {
  switch (choice) {
    case 1: return {80, 80, 255};     // red
    case 2: return {255, 160, 80};    // blue
    case 3: return {80, 255, 255};    // yellow
    case 4: return {255, 255, 255};   // white
    case 5: return {80, 180, 255};    // orange
    default: return {120, 255, 120};  // green
  }
}

class Draw final : public Block {
 public:
  Draw(const Values& q, const Config& config)
      : shape_(option(q, "shape")), colour_(colour(option(q, "color"))),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))),
        scale_(p(q, "scale") > 0.0 ? p(q, "scale") : 0.5) {
    const auto it = config.find("text");
    if (it != config.end()) text_ = it->second;
  }

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    cv::Mat out = as_bgr(*image).clone();

    const int u = in.contains("u") ? static_cast<int>(std::lround(get_or(in, "u"))) : out.cols / 2;
    const int v = in.contains("v") ? static_cast<int>(std::lround(get_or(in, "v"))) : out.rows / 2;
    const int width = std::max(1, static_cast<int>(std::lround(get_or(in, "width", 20.0))));
    const int height = std::max(1, static_cast<int>(std::lround(get_or(in, "height", 20.0))));

    switch (shape_) {
      case 0:  // box: u, v is the centre
        cv::rectangle(out, cv::Rect(u - width / 2, v - height / 2, width, height), colour_, thickness_, cv::LINE_AA);
        break;
      case 1:  // circle: width is the radius
        cv::circle(out, {u, v}, width, colour_, thickness_, cv::LINE_AA);
        break;
      case 2:  // cross: width is the arm
        cv::line(out, {u - width, v}, {u + width, v}, colour_, thickness_, cv::LINE_AA);
        cv::line(out, {u, v - width}, {u, v + width}, colour_, thickness_, cv::LINE_AA);
        break;
      default:  // text
        if (!text_.empty()) cv::putText(out, text_, {u, v}, cv::FONT_HERSHEY_SIMPLEX, scale_, colour_, thickness_, cv::LINE_AA);
        break;
    }
    return {{"image", image_out(std::move(out))}};
  }

 private:
  int shape_;
  cv::Scalar colour_;
  int thickness_;
  double scale_;
  std::string text_;
};

}  // namespace

void add_draw(Registry& registry) {
  Template t;
  t.name = "opencv/draw";
  t.def = {.type = "draw", .label = "Draw", .kind = Kind::Function,
           .inputs = {image_port("image"), {"u"}, {"v"}, {"width"}, {"height"}},
           .outputs = {image_port("image")},
           .params = {{.id = "shape", .type = "enum", .default_text = "box", .options = {"box", "circle", "cross", "text"}},
                      {.id = "color",
                       .type = "enum",
                       .default_text = "green",
                       .options = {"green", "red", "blue", "yellow", "white", "orange"}},
                      {"thickness", 1.0},
                      {.id = "text", .type = "text", .default_text = ""},
                      {"scale", 0.5}},
           .description = "Draws a box, circle, cross or text on an image (u, v centre; width, height or radius).",
           .cacheable = true};
  t.make = [](const Values& q, const Config& c) { return std::make_unique<Draw>(q, c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
