// Open: Removes small white specks.

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "common.hpp"
#include "opencv/morph_shared.hpp"

namespace paperboat::toolkit::opencv {
void add_open(Registry& registry) {
  add_morph(registry, "open", "Open (remove specks)", cv::MORPH_OPEN,
            "Removes small white specks.");
}

}  // namespace paperboat::toolkit::opencv
