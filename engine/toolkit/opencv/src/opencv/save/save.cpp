// Save: writes an image to a file (once, or every step).

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include "common.hpp"

namespace paperboat::toolkit::opencv {
namespace {

class Save final : public Block {
 public:
  Save(const Values& q, const Config& config) : once_(p(q, "once") != 0.0), quality_(p(q, "quality")) {
    const auto it = config.find("file");
    if (it != config.end()) file_ = it->second;
    const int format = option(q, "format");
    if (!file_.empty() && std::filesystem::path(file_).extension().empty())
      file_ += format == 1 ? ".jpg" : ".png";
  }

  Values step(const Context&, const Values& in) override {
    const cv::Mat* image = image_in(in, "image");
    if (!image || image->empty()) return {};
    if (file_.empty()) throw std::runtime_error("save needs a file");
    if (once_ && saved_) return {{"image", image_out(image->clone())}, {"saved", 1.0}, {"path", file_}};

    const std::filesystem::path path(file_);
    std::error_code ec;
    if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path(), ec);
    const bool jpeg = path.extension() == ".jpg" || path.extension() == ".jpeg";
    const std::vector<int> options =
        jpeg ? std::vector<int>{cv::IMWRITE_JPEG_QUALITY, std::clamp(static_cast<int>(quality_), 1, 100)}
             : std::vector<int>{};
    if (!cv::imwrite(file_, *image, options)) throw std::runtime_error("cannot write image \"" + file_ + "\"");
    saved_ = true;
    return {{"image", image_out(image->clone())}, {"saved", 1.0}, {"path", file_}};
  }

 private:
  std::string file_;
  bool once_;
  double quality_;
  bool saved_ = false;
};

}  // namespace

void add_save(Registry& registry) {
  Template t;
  t.name = "opencv/save";
  t.def = {.type = "save", .label = "Save image", .kind = Kind::Function,
           .inputs = {image_port("image")},
           .outputs = {image_port("image"), {"saved"}, {.id = "path", .type = "text"}},
           .params = {{.id = "file", .type = "file", .default_text = ""},
                      {.id = "format", .type = "enum", .default_text = "png", .options = {"png", "jpg"}},
                      {"quality", 90.0},
                      {"once", 1.0}},
           .description = "Writes its input to a picture file (once, or every step) and passes it on.",
           .cacheable = false};
  t.make = [](const Values& q, const Config& c) { return std::make_unique<Save>(q, c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::opencv
