// Canny: finds edges.

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
void add_canny(Registry& registry) {
  add_filter(registry, "canny", "Edges (Canny)", {{"low", 50.0}, {"high", 150.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::Canny(as_gray(im), out, p(q, "low"), p(q, "high"));
               return out;
             },
             "Finds edges.");
}

}  // namespace paperboat::toolkit::opencv
