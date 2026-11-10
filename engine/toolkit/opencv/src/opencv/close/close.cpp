// Close: Fills small black holes.

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
void add_close(Registry& registry) {
  add_morph(registry, "close", "Close (fill gaps)", cv::MORPH_CLOSE,
            "Fills small black holes.");
}

}  // namespace paperboat::toolkit::opencv
