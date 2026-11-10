// Erode: Shrinks white areas.

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
void add_erode(Registry& registry) {
  add_morph(registry, "erode", "Erode", cv::MORPH_ERODE,
            "Shrinks white areas.");
}

}  // namespace paperboat::toolkit::opencv
