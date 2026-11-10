#pragma once

// Helpers the opencv blocks share: a one-image-in/one-image-out block, parameter
// readers, and the image conversions every stage needs.

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace paperboat::toolkit::opencv {

// An odd kernel size, at least 1.
inline int odd(double size) { return std::max(1, static_cast<int>(std::lround(size)) | 1); }

// A number param, 0 when absent.
inline double p(const Values& params, std::string_view id) { return get_or(params, id); }

// An enum param as its index in the template's options.
inline int option(const Values& params, std::string_view id) { return static_cast<int>(get_or(params, id)); }

// Three channels (BGR), converting grey or BGRA input.
inline cv::Mat as_bgr(const cv::Mat& image) {
  if (image.channels() == 3) return image;
  cv::Mat bgr;
  if (image.channels() == 1) cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
  else if (image.channels() == 4) cv::cvtColor(image, bgr, cv::COLOR_BGRA2BGR);
  else return image;
  return bgr;
}

// An 8-bit copy, normalising a deeper image for display or for a stage that needs CV_8U.
inline cv::Mat as_8u(const cv::Mat& image) {
  if (image.depth() == CV_8U) return image;
  cv::Mat out;
  cv::normalize(image, out, 0, 255, cv::NORM_MINMAX, CV_8U);
  return out;
}

// A block that turns one image into another with a function.
class Filter final : public Block {
 public:
  using Fn = std::function<cv::Mat(const cv::Mat&, const Values& params)>;
  Filter(Fn fn, Values params) : fn_(std::move(fn)), params_(std::move(params)) {}
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    return {{"image", image_out(fn_(*image, params_))}};
  }

 private:
  Fn fn_;
  Values params_;
};

// The same, with the block's text config (a file or enum name) as well.
class ConfigFilter final : public Block {
 public:
  using Fn = std::function<cv::Mat(const cv::Mat&, const Values& params, const Config& config)>;
  ConfigFilter(Fn fn, Values params, Config config)
      : fn_(std::move(fn)), params_(std::move(params)), config_(std::move(config)) {}
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    return {{"image", image_out(fn_(*image, params_, config_))}};
  }

 private:
  Fn fn_;
  Values params_;
  Config config_;
};

inline void add_filter(Registry& registry, std::string type, std::string label, std::vector<ParamDef> params,
                       Filter::Fn fn, std::string description) {
  Template t;
  t.name = "opencv/" + type;
  t.def = {.type = std::move(type), .label = std::move(label), .kind = Kind::Function,
           .inputs = {image_port("image")}, .outputs = {image_port("image")}, .params = std::move(params),
           .description = std::move(description), .cacheable = true};
  t.make = [fn = std::move(fn)](const Values& q, const Config&) { return std::make_unique<Filter>(fn, q); };
  registry.add(std::move(t));
}

inline void add_config_filter(Registry& registry, std::string type, std::string label, std::vector<ParamDef> params,
                              ConfigFilter::Fn fn, std::string description) {
  Template t;
  t.name = "opencv/" + type;
  t.def = {.type = std::move(type), .label = std::move(label), .kind = Kind::Function,
           .inputs = {image_port("image")}, .outputs = {image_port("image")}, .params = std::move(params),
           .description = std::move(description), .cacheable = true};
  t.make = [fn = std::move(fn)](const Values& q, const Config& c) { return std::make_unique<ConfigFilter>(fn, q, c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
