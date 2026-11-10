// Bitwise: two images in, one out.

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
void add_bitwise(Registry& registry) {
  add_binary(registry, "bitwise", "Bitwise",
             Op::And, true,
             {{.id = "op",
               .type = "enum",
               .default_text = "and",
               .options = {"and", "or", "xor", "not", "add"}}},
             "Combines two images pixel by pixel (and with a grey second image: a mask).");
}

}  // namespace paperboat::toolkit::opencv
