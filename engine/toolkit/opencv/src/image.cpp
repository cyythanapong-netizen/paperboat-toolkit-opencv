// Images: loading and converting, and the helpers every block shares (data_url, as_gray, image_entity).

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace paperboat::toolkit::opencv {

cv::Mat load(const std::filesystem::path& file) {
  cv::Mat image = cv::imread(file.string(), cv::IMREAD_COLOR);
  if (image.empty()) throw std::runtime_error("cannot read image \"" + file.string() + "\"");
  return image;
}

namespace {

std::string base64(const std::vector<uchar>& bytes) {
  static constexpr char kAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string out;
  out.reserve((bytes.size() + 2) / 3 * 4);
  for (std::size_t i = 0; i < bytes.size(); i += 3) {
    const unsigned n = (unsigned{bytes[i]} << 16) | (i + 1 < bytes.size() ? unsigned{bytes[i + 1]} << 8 : 0u) |
                       (i + 2 < bytes.size() ? unsigned{bytes[i + 2]} : 0u);
    out += kAlphabet[(n >> 18) & 63];
    out += kAlphabet[(n >> 12) & 63];
    out += i + 1 < bytes.size() ? kAlphabet[(n >> 6) & 63] : '=';
    out += i + 2 < bytes.size() ? kAlphabet[n & 63] : '=';
  }
  return out;
}

}  // namespace

std::string data_url(const cv::Mat& image) {
  cv::Mat show;
  if (image.depth() != CV_8U) cv::normalize(image, show, 0, 255, cv::NORM_MINMAX, CV_8U);
  else show = image;
  if (show.channels() == 1) cv::cvtColor(show, show, cv::COLOR_GRAY2BGR);
  else if (show.channels() == 4) cv::cvtColor(show, show, cv::COLOR_BGRA2BGR);
  std::vector<uchar> jpeg;
  cv::imencode(".jpg", show, jpeg, {cv::IMWRITE_JPEG_QUALITY, 85});
  return "data:image/jpeg;base64," + base64(jpeg);
}

cv::Mat as_gray(const cv::Mat& image) {
  if (image.channels() == 1) return image;
  cv::Mat gray;
  if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
  else cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  return gray;
}

Entity image_entity(std::string id, const cv::Mat& image, double x, double y, double width) {
  const double height = width * image.rows / std::max(image.cols, 1);
  return {.id = std::move(id), .shape = "image", .pos = {x, y, 0}, .size = {width, height, 0}, .src = data_url(image)};
}

}  // namespace paperboat::toolkit::opencv
