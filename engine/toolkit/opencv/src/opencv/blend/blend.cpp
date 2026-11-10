// Blend: two images in, one out.

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"
#include "opencv/combine_shared.hpp"

namespace paperboat::toolkit::opencv {
void add_blend(Registry& registry) {
  add_binary(registry, "blend", "Blend", Op::Blend, false, {{"alpha", 0.5}},
             "Mixes two images: alpha of the first, the rest of the second.");
}

}  // namespace paperboat::toolkit::opencv
