// Colour and tone: convert the colour space, stretch the range, equalise the histogram.

#include <algorithm>
#include <cmath>
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

int norm_type(const Values& q) {
  switch (option(q, "mode")) {
    case 1: return cv::NORM_L1;
    case 2: return cv::NORM_L2;
    case 3: return cv::NORM_INF;
    default: return cv::NORM_MINMAX;
  }
}

cv::Mat normalize(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const double alpha = p(q, "alpha");
  const double beta = p(q, "beta");
  cv::normalize(image, out, alpha, beta, norm_type(q), CV_8U);
  return out;
}

cv::Mat equalize(const cv::Mat& image, const Values&) {
  cv::Mat out;
  cv::equalizeHist(as_gray(image), out);
  return out;
}

cv::Mat clahe(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const double clip = p(q, "clip") > 0.0 ? p(q, "clip") : 2.0;
  const int tiles = std::max(1, static_cast<int>(std::lround(p(q, "tiles"))));
  cv::Ptr<cv::CLAHE> enhance = cv::createCLAHE(clip, {tiles, tiles});
  enhance->apply(as_gray(image), out);
  return out;
}

}  // namespace

void add_color(Registry& registry) {
  add_filter(registry, "convert", "Convert colour",
             {{.id = "space",
               .type = "enum",
               .default_text = "gray",
               .options = {"gray", "bgr", "rgb", "hsv", "hls", "lab"}}},
             convert, "Changes the colour space (grey, BGR, RGB, HSV, HLS, Lab).");

  add_filter(registry, "normalize", "Normalize",
             {{"alpha", 0.0},
              {"beta", 255.0},
              {.id = "mode", .type = "enum", .default_text = "minmax", .options = {"minmax", "l1", "l2", "inf"}}},
             normalize, "Stretches an image's values into a range.");

  add_filter(registry, "equalize", "Equalize", {}, equalize,
             "Spreads the histogram: more contrast in a flat or dark image.");

  add_filter(registry, "clahe", "CLAHE", {{"clip", 2.0}, {"tiles", 8.0}}, clahe,
             "Local histogram equalisation (CLAHE), for uneven lighting.");
}

}  // namespace paperboat::toolkit::opencv
