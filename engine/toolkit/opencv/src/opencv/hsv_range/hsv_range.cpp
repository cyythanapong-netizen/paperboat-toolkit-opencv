// HSV range: keeps the pixels within a range of colour.

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
void add_hsv_range(Registry& registry) {
  // A mask of the pixels in an HSV box. Hue is 0–180; h_min > h_max wraps around red.
  add_filter(registry, "hsv_range", "HSV range",
             {{"h_min", 0.0}, {"h_max", 180.0}, {"s_min", 0.0}, {"s_max", 255.0}, {"v_min", 0.0}, {"v_max", 255.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat hsv;
               cv::Mat bgr = im;
               if (im.channels() == 1) cv::cvtColor(im, bgr, cv::COLOR_GRAY2BGR);
               cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
               auto range = [&](double a, double b) {
                 cv::Mat m;
                 cv::inRange(hsv, cv::Scalar(a, p(q, "s_min"), p(q, "v_min")),
                             cv::Scalar(b, p(q, "s_max"), p(q, "v_max")), m);
                 return m;
               };
               const double h0 = p(q, "h_min"), h1 = p(q, "h_max");
               return h0 <= h1 ? range(h0, h1) : cv::Mat(range(h0, 180) | range(0, h1));
             },
             "Keeps the pixels within a range of colour.");
}

}  // namespace paperboat::toolkit::opencv
