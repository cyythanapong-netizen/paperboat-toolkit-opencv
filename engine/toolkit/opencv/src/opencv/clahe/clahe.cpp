// Clahe: colour and tone.

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

cv::Mat clahe(const cv::Mat& image, const Values& q) {
  cv::Mat out;
  const double clip = p(q, "clip") > 0.0 ? p(q, "clip") : 2.0;
  const int tiles = std::max(1, static_cast<int>(std::lround(p(q, "tiles"))));
  cv::Ptr<cv::CLAHE> enhance = cv::createCLAHE(clip, {tiles, tiles});
  enhance->apply(as_gray(image), out);
  return out;
}

}  // namespace

void add_clahe(Registry& registry) {
  add_filter(registry, "clahe", "CLAHE", {{"clip", 2.0}, {"tiles", 8.0}}, clahe,
             "Local histogram equalisation (CLAHE), for uneven lighting.");
}

}  // namespace paperboat::toolkit::opencv
