// Analysis: image statistics, contours, lines, circles and template matching.

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

namespace paperboat::toolkit::opencv {
namespace {

// The largest contour of a set, or null.
const std::vector<cv::Point>* largest(const std::vector<const std::vector<cv::Point>*>& contours) {
  const std::vector<cv::Point>* best = nullptr;
  double best_area = -1.0;
  for (const auto* contour : contours) {
    const double area = cv::contourArea(*contour);
    if (area > best_area) {
      best_area = area;
      best = contour;
    }
  }
  return best;
}

class Stats final : public Block {
 public:
  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    cv::Scalar mean;
    cv::Scalar stddev;
    cv::meanStdDev(gray, mean, stddev);
    double min = 0.0;
    double max = 0.0;
    cv::minMaxLoc(gray, &min, &max);
    return {{"mean", mean[0]},
            {"std", stddev[0]},
            {"min", min},
            {"max", max},
            {"nonzero", static_cast<double>(cv::countNonZero(gray))},
            {"pixels", static_cast<double>(gray.total())}};
  }
};

class Contours final : public Block {
 public:
  explicit Contours(const Values& q)
      : min_area_(p(q, "min_area")), max_area_(p(q, "max_area")), draw_(option(q, "draw")),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<std::vector<cv::Point>> all;
    cv::findContours(gray.clone(), all, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<const std::vector<cv::Point>*> kept;
    for (const auto& contour : all) {
      const double area = cv::contourArea(contour);
      if (area < min_area_ || (max_area_ > 0.0 && area > max_area_)) continue;
      kept.push_back(&contour);
    }

    cv::Mat marked = as_bgr(gray).clone();
    if (draw_ == 0) {
      for (const auto* contour : kept) cv::drawContours(marked, std::vector{*contour}, -1, cv::Scalar(120, 255, 120),
                                                        thickness_, cv::LINE_AA);
    }

    const std::vector<cv::Point>* best = largest(kept);
    if (!best) {
      return {{"image", image_out(std::move(marked))}, {"count", static_cast<double>(kept.size())},
              {"u", 0.0}, {"v", 0.0}, {"area", 0.0}, {"width", 0.0}, {"height", 0.0}, {"found", 0.0}};
    }

    const double area = cv::contourArea(*best);
    const cv::Moments moments = cv::moments(*best, false);
    const double u = moments.m00 > 0.0 ? moments.m10 / moments.m00 : 0.0;
    const double v = moments.m00 > 0.0 ? moments.m01 / moments.m00 : 0.0;
    const cv::Rect box = cv::boundingRect(*best);
    if (draw_ != 2) {
      cv::rectangle(marked, box, cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
      cv::circle(marked, {static_cast<int>(std::lround(u)), static_cast<int>(std::lround(v))}, 4,
                 cv::Scalar(255, 160, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(kept.size())},
            {"u", u},
            {"v", v},
            {"area", area},
            {"width", static_cast<double>(box.width)},
            {"height", static_cast<double>(box.height)},
            {"found", 1.0}};
  }

 private:
  double min_area_;
  double max_area_;
  int draw_;  // 0 all, 1 largest, 2 none
  int thickness_;
};

class HoughLines final : public Block {
 public:
  explicit HoughLines(const Values& q)
      : threshold_(std::max(1, static_cast<int>(std::lround(p(q, "threshold"))))),
        min_length_(std::max(0.0, p(q, "min_length"))),
        max_gap_(std::max(0.0, p(q, "max_gap"))),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(gray, lines, 1.0, CV_PI / 180.0, threshold_, min_length_, max_gap_);

    cv::Mat marked = as_bgr(gray).clone();
    double longest = 0.0;
    double angle = 0.0;
    for (const auto& line : lines) {
      const double dx = line[2] - line[0];
      const double dy = line[3] - line[1];
      const double length = std::hypot(dx, dy);
      if (length > longest) {
        longest = length;
        angle = std::atan2(dy, dx) * 180.0 / CV_PI;
      }
      cv::line(marked, {line[0], line[1]}, {line[2], line[3]}, cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(lines.size())},
            {"angle", angle},
            {"length", longest}};
  }

 private:
  int threshold_;
  double min_length_;
  double max_gap_;
  int thickness_;
};

class HoughCircles final : public Block {
 public:
  explicit HoughCircles(const Values& q)
      : dp_(p(q, "dp") > 0.0 ? p(q, "dp") : 1.2),
        min_dist_(p(q, "min_dist") > 0.0 ? p(q, "min_dist") : 20.0),
        param1_(std::max(1.0, p(q, "param1"))),
        param2_(std::max(1.0, p(q, "param2"))),
        min_radius_(std::max(0.0, p(q, "min_radius"))),
        max_radius_(std::max(0.0, p(q, "max_radius"))),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {}

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    const cv::Mat gray = as_8u(as_gray(*image));
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, dp_, min_dist_, param1_, param2_,
                     static_cast<int>(std::lround(min_radius_)), static_cast<int>(std::lround(max_radius_)));

    cv::Mat marked = as_bgr(gray).clone();
    double radius = 0.0;
    double u = 0.0;
    double v = 0.0;
    for (const auto& circle : circles) {
      if (circle[2] > radius) {
        radius = circle[2];
        u = circle[0];
        v = circle[1];
      }
      cv::circle(marked, {static_cast<int>(std::lround(circle[0])), static_cast<int>(std::lround(circle[1]))},
                 static_cast<int>(std::lround(circle[2])), cv::Scalar(120, 255, 120), thickness_, cv::LINE_AA);
      cv::circle(marked, {static_cast<int>(std::lround(circle[0])), static_cast<int>(std::lround(circle[1]))}, 2,
                 cv::Scalar(255, 160, 120), thickness_, cv::LINE_AA);
    }
    return {{"image", image_out(std::move(marked))},
            {"count", static_cast<double>(circles.size())},
            {"u", u},
            {"v", v},
            {"radius", radius}};
  }

 private:
  double dp_;
  double min_dist_;
  double param1_;
  double param2_;
  double min_radius_;
  double max_radius_;
  int thickness_;
};

class TemplateMatch final : public Block {
 public:
  TemplateMatch(const Values& q, const Config& config)
      : threshold_(p(q, "threshold")), method_(option(q, "method")),
        thickness_(std::max(1, static_cast<int>(std::lround(p(q, "thickness"))))) {
    const auto it = config.find("template");
    if (it != config.end() && !it->second.empty()) template_ = load(it->second);
  }

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    if (template_.empty()) throw std::runtime_error("template_match needs a template file");
    const cv::Mat gray = as_8u(as_gray(*image));
    const cv::Mat patch = as_8u(as_gray(template_));
    if (patch.cols > gray.cols || patch.rows > gray.rows)
      throw std::runtime_error("template_match: the template is larger than the image");

    const int method = method_ == 0 ? cv::TM_SQDIFF_NORMED
                       : method_ == 1 ? cv::TM_CCORR_NORMED
                                      : cv::TM_CCOEFF_NORMED;
    cv::Mat result;
    cv::matchTemplate(gray, patch, result, method);
    double min_value = 0.0;
    double max_value = 0.0;
    cv::Point min_location;
    cv::Point max_location;
    cv::minMaxLoc(result, &min_value, &max_value, &min_location, &max_location);
    const bool lower_is_better = method_ == 0;
    const double score = lower_is_better ? 1.0 - min_value : max_value;
    const cv::Point at = lower_is_better ? min_location : max_location;
    const bool found = score >= threshold_;

    cv::Mat marked = as_bgr(gray).clone();
    const cv::Rect box(at.x, at.y, patch.cols, patch.rows);
    cv::rectangle(marked, box, found ? cv::Scalar(120, 255, 120) : cv::Scalar(120, 120, 255), thickness_, cv::LINE_AA);
    return {{"image", image_out(std::move(marked))},
            {"score", score},
            {"u", static_cast<double>(at.x) + patch.cols / 2.0},
            {"v", static_cast<double>(at.y) + patch.rows / 2.0},
            {"width", static_cast<double>(patch.cols)},
            {"height", static_cast<double>(patch.rows)},
            {"found", found ? 1.0 : 0.0}};
  }

 private:
  double threshold_;
  int method_;
  int thickness_;
  cv::Mat template_;
};

void add_stats(Registry& registry) {
  Template t;
  t.name = "opencv/stats";
  t.def = {.type = "stats", .label = "Image stats", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {{"mean"}, {"std"}, {"min"}, {"max"}, {"nonzero"}, {"pixels"}},
           .params = {}, .description = "Mean, spread, range and non-black count of an image.", .cacheable = true};
  t.make = [](const Values&, const Config&) { return std::make_unique<Stats>(); };
  registry.add(std::move(t));
}

void add_contours(Registry& registry) {
  Template t;
  t.name = "opencv/contours";
  t.def = {.type = "contours", .label = "Contours", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"u"}, {"v"}, {"area"}, {"width"}, {"height"}, {"found"}},
           .params = {{"min_area", 0.0},
                      {"max_area", 0.0},
                      {.id = "draw", .type = "enum", .default_text = "all", .options = {"all", "largest", "none"}},
                      {"thickness", 1.0}},
           .description = "The white blobs' count, the largest one's centre, area and box, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<Contours>(q); };
  registry.add(std::move(t));
}

void add_hough_lines(Registry& registry) {
  Template t;
  t.name = "opencv/hough_lines";
  t.def = {.type = "hough_lines", .label = "Hough lines", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"angle"}, {"length"}},
           .params = {{"threshold", 80.0}, {"min_length", 30.0}, {"max_gap", 5.0}, {"thickness", 1.0}},
           .description = "Finds straight lines: how many, the longest one's angle and length, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<HoughLines>(q); };
  registry.add(std::move(t));
}

void add_hough_circles(Registry& registry) {
  Template t;
  t.name = "opencv/hough_circles";
  t.def = {.type = "hough_circles", .label = "Hough circles", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"count"}, {"u"}, {"v"}, {"radius"}},
           .params = {{"dp", 1.2},       {"min_dist", 20.0}, {"param1", 100.0},
                      {"param2", 30.0},  {"min_radius", 0.0}, {"max_radius", 0.0},
                      {"thickness", 1.0}},
           .description = "Finds circles: how many, the largest one's centre and radius, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config&) { return std::make_unique<HoughCircles>(q); };
  registry.add(std::move(t));
}

void add_template_match(Registry& registry) {
  Template t;
  t.name = "opencv/template_match";
  t.def = {.type = "template_match", .label = "Template match", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"score"}, {"u"}, {"v"}, {"width"}, {"height"}, {"found"}},
           .params = {{.id = "method",
                       .type = "enum",
                       .default_text = "ccoeff",
                       .options = {"sqdiff", "ccorr", "ccoeff"}},
                      {"threshold", 0.8},
                      {"thickness", 2.0}},
           .description = "Finds a template picture in an image: where, how well, and a marked image.",
           .cacheable = true};
  t.make = [](const Values& q, const Config& c) { return std::make_unique<TemplateMatch>(q, c); };
  registry.add(std::move(t));
}

}  // namespace

void add_analyze(Registry& registry) {
  add_stats(registry);
  add_contours(registry);
  add_hough_lines(registry);
  add_hough_circles(registry);
  add_template_match(registry);
}

}  // namespace paperboat::toolkit::opencv
