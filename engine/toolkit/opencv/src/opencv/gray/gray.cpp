// Gray: turns an image to grey.

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
void add_gray(Registry& registry) {
  add_filter(registry, "gray", "Gray", {}, [](const cv::Mat& im, const Values&) { return as_gray(im); },
             "Turns an image to grey.");
}

}  // namespace paperboat::toolkit::opencv
