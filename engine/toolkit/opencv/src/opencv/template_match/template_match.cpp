// template_match: image analysis.

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

class TemplateMatch final : public Block {
 public:
  TemplateMatch(const Values& q, const Config& config)
      : threshold_(p(q, "threshold")), method_(option(q, "method")),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {
    const auto it = config.find("template");
    if (it != config.end() && !it->second.empty()) template_ = load(it->second);
  }

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    if (template_.empty()) throw std::runtime_error("template_match needs a template file");
    const cv::Mat gray = as_8u(as_gray(*image));
    const cv::Mat patch = as_8u(as_gray(template_));
    if (patch.cols > gray.cols || patch.rows > gray.rows)
      throw std::runtime_error("template_match: the template is larger than the image");

    const int method = method_ == 0 ? cv::TM_SQDIFF_NORMED
                       : method_ == 1 ? cv::TM_CCORR_NORMED
                                      : cv::TM_CCOEFF_NORMED;
    cv::Mat result;
    cv::matchTemplate(gray, patch, result, method);
    double min_value = 0.0;
    double max_value = 0.0;
    cv::Point min_location;
    cv::Point max_location;
    cv::minMaxLoc(result, &min_value, &max_value, &min_location, &max_location);
    const bool lower_is_better = method_ == 0;
    const double score = lower_is_better ? 1.0 - min_value : max_value;
    const cv::Point at = lower_is_better ? min_location : max_location;
    const bool found = score >= threshold_;

    cv::Mat marked = as_bgr(gray).clone();
    const cv::Rect box(at.x, at.y, patch.cols, patch.rows);
    cv::rectangle(marked, box, found ? cv::Scalar(120, 255, 120) : cv::Scalar(120, 120, 255), thickness_, cv::LINE_AA);
    return {{"image", image_out(std::move(marked))},
            {"score", score},
            {"u", static_cast<double>(at.x) + patch.cols / 2.0},
            {"v", static_cast<double>(at.y) + patch.rows / 2.0},
            {"width", static_cast<double>(patch.cols)},
            {"height", static_cast<double>(patch.rows)},
            {"found", found ? 1.0 : 0.0}};
  }

 private:
  double threshold_;
  int method_;
  int thickness_;
  cv::Mat template_;
};

}  // namespace

void add_template_match(Registry& registry) {
  Template t;
  t.name = "opencv/template_match";
  t.def = {.type = "template_match", .label = "Template match", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"score"}, {"u"}, {"v"}, {"width"}, {"height"}, {"found"}},
           .params = {{.id = "method",
                       .type = "enum",
                       .default_text = "ccoeff",
                       .options = {"sqdiff", "ccorr", "ccoeff"}},
                      {"threshold", 0.8},
                      {"thickness", 2.0}},
           .description = "Finds a template picture in an image: where, how well, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config& c) { return std::make_unique<TemplateMatch>(q, c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
