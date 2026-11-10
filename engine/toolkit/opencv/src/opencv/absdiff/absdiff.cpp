// Absdiff: two images in, one out.

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
void add_absdiff(Registry& registry) {
  add_binary(registry, "absdiff", "Difference", Op::AbsDiff, false, {},
             "How much two images differ: motion against a background, a before and after.");
}

}  // namespace paperboat::toolkit::opencv
