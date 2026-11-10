#pragma once

// Shared by bitwise, blend and absdiff: two images in, one out, by an operation.

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {

enum class Op { And, Or, Xor, Not, Add, Blend, AbsDiff };

// Two images: `image` and `image2`. `not` needs only the first; a 1-channel `image2` with
// `and` is a mask on `image`.
class Binary final : public Block {
 public:
  Binary(Op op, double alpha) : op_(op), alpha_(alpha) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* a = image_in(in, "image");
    if (!a || a->empty()) return {};
    const cv::Mat* b = image_in(in, "image2");

    if (op_ == Op::Not) {
      cv::Mat out;
      cv::bitwise_not(*a, out);
      return {{"image", image_out(std::move(out))}};
    }
    if (!b || b->empty()) return {};
    if (a->size() != b->size()) throw std::runtime_error("the two images must have the same size");
    if (op_ == Op::And && b->channels() == 1 && a->channels() != 1) {
      cv::Mat out;
      cv::bitwise_and(*a, *a, out, *b);
      return {{"image", image_out(std::move(out))}};
    }
    if (a->type() != b->type()) throw std::runtime_error("the two images must have the same type");

    cv::Mat out;
    switch (op_) {
      case Op::And: cv::bitwise_and(*a, *b, out); break;
      case Op::Or: cv::bitwise_or(*a, *b, out); break;
      case Op::Xor: cv::bitwise_xor(*a, *b, out); break;
      case Op::Add: cv::add(*a, *b, out); break;
      case Op::Blend: {
        const double alpha = std::clamp(alpha_, 0.0, 1.0);
        cv::addWeighted(*a, alpha, *b, 1.0 - alpha, 0.0, out);
        break;
      }
      case Op::AbsDiff: cv::absdiff(*a, *b, out); break;
      case Op::Not: break;  // handled above
    }
    return {{"image", image_out(std::move(out))}};
  }

 private:
  Op op_;
  double alpha_;
};

inline void add_binary(Registry& registry, std::string type, std::string label, Op op, bool from_param,
                std::vector<ParamDef> params, std::string description) {
  Template t;
  t.name = "opencv/" + type;
  t.def = {.type = std::move(type), .label = std::move(label), .kind = Kind::Function,
           .inputs = {image_port("image"), image_port("image2")}, .outputs = {image_port("image")},
           .params = std::move(params), .description = std::move(description), .cacheable = true};
  t.make = [op, from_param](const Values& q, const Config&) {
    const Op chosen = from_param ? static_cast<Op>(std::clamp(option(q, "op"), 0, 4)) : op;
    return std::make_unique<Binary>(chosen, p(q, "alpha"));
  };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
