// Convert: colour and tone.

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

cv::Mat convert(const cv::Mat& image, const Values& q) {
  switch (option(q, "space")) {
    case 0: return as_gray(image);
    case 1: return as_bgr(image);
    case 2: {
      cv::Mat out;
      cv::cvtColor(as_bgr(image), out, cv::COLOR_BGR2RGB);
      return out;
    }
    case 3: {
      cv::Mat out;
      cv::cvtColor(as_bgr(image), out, cv::COLOR_BGR2HSV);
      return out;
    }
    case 4: {
      cv::Mat out;
      cv::cvtColor(as_bgr(image), out, cv::COLOR_BGR2HLS);
      return out;
    }
    case 5: {
      cv::Mat out;
      cv::cvtColor(as_bgr(image), out, cv::COLOR_BGR2Lab);
      return out;
    }
    default: return as_bgr(image);
  }
}

}  // namespace

void add_convert(Registry& registry) {
  add_filter(registry, "convert", "Convert colour",
             {{.id = "space",
               .type = "enum",
               .default_text = "gray",
               .options = {"gray", "bgr", "rgb", "hsv", "hls", "lab"}}},
             convert, "Changes the colour space (grey, BGR, RGB, HSV, HLS, Lab).");
}

}  // namespace paperboat::toolkit::opencv
