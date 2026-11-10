// ocv-01-parts-counter: count the parts on a synthetic conveyor with the optional opencv
// toolkit. The graph evaluates once, so the example is deterministic and needs no clock.

#include <paperboat/sdk.hpp>

#include "domain/scene.hpp"
#include "paperboat/toolkit/opencv.hpp"

int main(int argc, char** argv) {
  static const paperboat::Registry registry = [] {
    paperboat::Registry r = paperboat::builtins();
    paperboat::toolkit::opencv::register_opencv(r);
    parts_counter::register_scene(r);
    return r;
  }();
  static const paperboat::Catalog catalog = paperboat::load_descriptor_file(PAPERBOAT_DESCRIPTOR, registry);
  const paperboat::Backend backend{
      .engine = "ocv-01-parts-counter",
      .catalog = &catalog,
      .view = {{"mode", "2d"}, {"center", {0, 0}}, {"height", 8.0}},
      .run = {.mode = paperboat::RunSettings::Mode::Once, .real_time = false},
  };
  return paperboat::main(backend, argc, argv, 8781);
}
