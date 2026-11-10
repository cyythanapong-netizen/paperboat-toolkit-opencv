// The opencv package: each block is a folder, opencv/<block>/<block>.cpp, defining add_<block>
// (image is the Image station object). This registers them.

#include "paperboat/toolkit/opencv.hpp"

namespace paperboat::toolkit::opencv {

void add_absdiff(Registry&);
void add_adaptive_threshold(Registry&);
void add_background(Registry&);
void add_bitwise(Registry&);
void add_blend(Registry&);
void add_blur(Registry&);
void add_canny(Registry&);
void add_centroid(Registry&);
void add_clahe(Registry&);
void add_close(Registry&);
void add_contours(Registry&);
void add_convert(Registry&);
void add_crop(Registry&);
void add_dilate(Registry&);
void add_distance(Registry&);
void add_draw(Registry&);
void add_equalize(Registry&);
void add_erode(Registry&);
void add_flip(Registry&);
void add_gray(Registry&);
void add_hough_circles(Registry&);
void add_hough_lines(Registry&);
void add_hsv_range(Registry&);
void add_image(Registry&);
void add_invert(Registry&);
void add_laplacian(Registry&);
void add_median(Registry&);
void add_normalize(Registry&);
void add_open(Registry&);
void add_resize(Registry&);
void add_rotate(Registry&);
void add_save(Registry&);
void add_sobel(Registry&);
void add_stats(Registry&);
void add_template_match(Registry&);
void add_threshold(Registry&);
void add_view(Registry&);

void register_opencv(Registry& registry) {
  add_absdiff(registry);
  add_adaptive_threshold(registry);
  add_background(registry);
  add_bitwise(registry);
  add_blend(registry);
  add_blur(registry);
  add_canny(registry);
  add_centroid(registry);
  add_clahe(registry);
  add_close(registry);
  add_contours(registry);
  add_convert(registry);
  add_crop(registry);
  add_dilate(registry);
  add_distance(registry);
  add_draw(registry);
  add_equalize(registry);
  add_erode(registry);
  add_flip(registry);
  add_gray(registry);
  add_hough_circles(registry);
  add_hough_lines(registry);
  add_hsv_range(registry);
  add_image(registry);
  add_invert(registry);
  add_laplacian(registry);
  add_median(registry);
  add_normalize(registry);
  add_open(registry);
  add_resize(registry);
  add_rotate(registry);
  add_save(registry);
  add_sobel(registry);
  add_stats(registry);
  add_template_match(registry);
  add_threshold(registry);
  add_view(registry);
}

}  // namespace paperboat::toolkit::opencv
