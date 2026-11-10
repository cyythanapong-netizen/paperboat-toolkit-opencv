// The opencv package's classic stages: colour, filters, thresholding, morphology, edges.
// Each takes `image` in and gives `image` out; an unwired input gives no output. The
// specialised stages live next to this file (geometry, color, gradient, combine, analyze,
// draw, io, background).

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {

void add_image_object(Registry& registry);  // image.cpp
void add_centroid(Registry& registry);      // centroid.cpp
void add_geometry(Registry& registry);      // geometry.cpp
void add_color(Registry& registry);         // color.cpp
void add_gradient(Registry& registry);      // gradient.cpp
void add_combine(Registry& registry);       // combine.cpp
void add_analyze(Registry& registry);       // analyze.cpp
void add_draw(Registry& registry);          // draw.cpp
void add_io(Registry& registry);            // io.cpp
void add_background(Registry& registry);    // background.cpp

namespace {

cv::Mat morph(const cv::Mat& image, const Values& q, cv::MorphTypes op) {
  const int k = odd(p(q, "size"));
  cv::Mat out;
  cv::morphologyEx(image, out, op, cv::getStructuringElement(cv::MORPH_ELLIPSE, {k, k}), {-1, -1},
                   std::max(1, static_cast<int>(p(q, "iterations"))));
  return out;
}

// Shows its input image on the Dashboard: an image instrument.
struct View final : Block {
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image) return {};
    return {{"value", data_url(*image)}};
  }
};

}  // namespace

void register_opencv(Registry& registry) {
  add_image_object(registry);
  add_centroid(registry);
  add_geometry(registry);
  add_color(registry);
  add_gradient(registry);
  add_combine(registry);
  add_analyze(registry);
  add_draw(registry);
  add_io(registry);
  add_background(registry);

  add_filter(registry, "gray", "Gray", {}, [](const cv::Mat& im, const Values&) { return as_gray(im); },
             "Turns an image to grey.");

  add_filter(registry, "blur", "Blur (Gaussian)", {{"size", 5.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               const int k = odd(p(q, "size"));
               cv::GaussianBlur(im, out, {k, k}, 0);
               return out;
             },
             "Softens an image.");

  add_filter(registry, "median", "Blur (median)", {{"size", 5.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::medianBlur(im, out, std::max(3, odd(p(q, "size"))));
               return out;
             },
             "Removes speckle noise, keeping edges.");

  // Gray → black and white at `level` (0–255). otsu = 1 picks the level from the picture;
  // invert = 1 makes dark things white (parts on a light background).
  add_filter(registry, "threshold", "Threshold", {{"level", 128.0}, {"otsu", 1.0}, {"invert", 0.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               const int type = (p(q, "invert") > 0 ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY) |
                                (p(q, "otsu") > 0 ? cv::THRESH_OTSU : 0);
               cv::threshold(as_gray(im), out, p(q, "level"), 255, type);
               return out;
             },
             "Splits an image into black and white at a level.");

  // Threshold against the local mean: for uneven lighting.
  add_filter(registry, "adaptive_threshold", "Threshold (adaptive)", {{"size", 31.0}, {"c", 5.0}, {"invert", 0.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::adaptiveThreshold(as_gray(im), out, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                                     p(q, "invert") > 0 ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY,
                                     std::max(3, odd(p(q, "size"))), p(q, "c"));
               return out;
             },
             "Black and white by local brightness: for uneven light.");

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

  add_filter(registry, "canny", "Edges (Canny)", {{"low", 50.0}, {"high", 150.0}},
             [](const cv::Mat& im, const Values& q) {
               cv::Mat out;
               cv::Canny(as_gray(im), out, p(q, "low"), p(q, "high"));
               return out;
             },
             "Finds edges.");

  for (auto [type, label, op] : {std::tuple{"erode", "Erode", cv::MORPH_ERODE},
                                 {"dilate", "Dilate", cv::MORPH_DILATE},
                                 {"open", "Open (remove specks)", cv::MORPH_OPEN},
                                 {"close", "Close (fill gaps)", cv::MORPH_CLOSE}}) {
    const std::string_view type_name(type);
    const std::string description = type_name == "erode"   ? "Shrinks white areas."
                                    : type_name == "dilate" ? "Grows white areas."
                                    : type_name == "open"   ? "Removes small white specks."
                                                            : "Fills small black holes.";
    add_filter(registry, type, label, {{"size", 5.0}, {"iterations", 1.0}},
               [op = op](const cv::Mat& im, const Values& q) { return morph(im, q, op); }, description);
  }

  add_filter(registry, "invert", "Invert", {},
             [](const cv::Mat& im, const Values&) {
               cv::Mat out;
               cv::bitwise_not(im, out);
               return out;
             },
             "Swaps black and white.");

  // How far each white pixel is from black: touching blobs get one peak each.
  add_filter(registry, "distance", "Distance", {},
             [](const cv::Mat& im, const Values&) {
               cv::Mat dist, out;
               cv::distanceTransform(as_gray(im), dist, cv::DIST_L2, 5);
               cv::normalize(dist, out, 0, 255, cv::NORM_MINMAX, CV_8U);
               return out;
             },
             "How far each white pixel is from black.");

  Template view;
  view.name = "opencv/view";
  view.def = {.type = "view", .label = "Image view", .kind = Kind::Monitor, .inputs = {image_port("image")},
              .outputs = {}, .params = {}, .instrument = "image", .description = "Shows an image on the Dashboard."};
  view.make = [](const Values&, const Config&) { return std::make_unique<View>(); };
  registry.add(std::move(view));
}

}  // namespace paperboat::toolkit::opencv
