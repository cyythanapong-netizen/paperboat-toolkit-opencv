// ocv-03-image-lab: a synthetic gradient card through resize, crop, equalize and Sobel,
// with image statistics. The graph evaluates once.

#include <paperboat/sdk.hpp>

#include "domain/scene.hpp"
#include "paperboat/toolkit/opencv.hpp"

int main(int argc, char** argv) {
  static const paperboat::Registry registry = [] {
    paperboat::Registry r = paperboat::builtins();
    paperboat::toolkit::opencv::register_opencv(r);
    image_lab::register_scene(r);
    return r;
  }();
  static const paperboat::Catalog catalog = paperboat::load_descriptor_file(PAPERBOAT_DESCRIPTOR, registry);
  const paperboat::Backend backend{
      .engine = "ocv-03-image-lab",
      .catalog = &catalog,
      .view = {{"mode", "2d"}, {"center", {0, 0}}, {"height", 8.0}},
      .run = {.mode = paperboat::RunSettings::Mode::Once, .real_time = false},
  };
  return paperboat::main(backend, argc, argv, 8783);
}
