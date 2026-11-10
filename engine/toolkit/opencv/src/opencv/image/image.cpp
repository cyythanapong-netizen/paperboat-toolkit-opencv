// The Image station object: a picture on the Station, with its linked block.

#include <memory>
#include <string>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "paperboat/toolkit/opencv.hpp"

namespace paperboat::toolkit::opencv {
namespace {

// A picture on the Station: the file named by its `file` property, `width` units across.
class ImageObject final : public StationObject {
 public:
  explicit ImageObject(const Values& props)
      : file_(get_text(props, "file")), width_(get_or(props, "width", 6)), image_(load(file_)) {}

  Values read() const override { return {{"image", image_out(image_)}}; }
  void step(const Context&, const Values&) override {}
  std::vector<Entity> entities(const std::string& id, double z) const override {
    auto e = image_entity(id, image_, 0, 0, width_);
    e.pos[2] = z;
    return {e};
  }

 private:
  std::string file_;
  double width_;
  cv::Mat image_;
};

}  // namespace

void add_image(Registry& registry) {
  registry.add(StationTemplate{
      .name = "opencv/image",
      .def = {.type = "image",
              .label = "Image",
              .role = "body",
               .props = {{.id = "file", .default_value = 0, .type = "file", .default_text = ""}, {"width", 6.0}},
               .links = {},
               .block = BlockDef{.type = "image",
                                 .label = "Image",
                                 .kind = Kind::Probe,
                                 .inputs = {},
                                 .outputs = {image_port("image")},
                                 .params = {},
                                 .description = "A picture from a file, for a flow to work on."},
               .description = "A picture from a file, for a flow to work on."},
      .make = [](const Values& props) { return std::make_unique<ImageObject>(props); },
  });
}

}  // namespace paperboat::toolkit::opencv
