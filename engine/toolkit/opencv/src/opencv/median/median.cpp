// Blur (median): removes speckle noise, keeping edges.

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
void add_median(Registry& registry) {
  add_filter(registry, "median", "Blur (median)", {{"size", 5.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::medianBlur(im, out, std::max(3, odd(p(q, "size"))));
               return out;
             },
             "Removes speckle noise, keeping edges.");
}

}  // namespace paperboat::toolkit::opencv
