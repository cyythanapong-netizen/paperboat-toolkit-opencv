# paperboat-toolkit-opencv

The optional image-processing toolkit for [Paperboat](https://github.com/paperboatlab/paperboat).

It provides the `opencv` package: classic image stages on `image` wires, an `Image` station
object, an `image` Dashboard instrument, and three examples. It depends on OpenCV and is kept
out of the core engine so the main build stays light.

## Blocks

| Group | Blocks |
| --- | --- |
| Geometry | `resize`, `crop`, `flip`, `rotate` |
| Colour and tone | `gray`, `convert`, `normalize`, `equalize`, `clahe` |
| Filters and edges | `blur`, `median`, `threshold`, `adaptive_threshold`, `hsv_range`, `canny`, `sobel`, `laplacian`, `erode`, `dilate`, `open`, `close`, `invert`, `distance` |
| Image algebra | `bitwise`, `blend`, `absdiff`, `background` |
| Analysis | `stats`, `centroid`, `contours`, `hough_lines`, `hough_circles`, `template_match` |
| Annotation and output | `draw`, `save` |
| Instrument | `view` (the `image` Dashboard instrument) |
| Station object | `image` (a picture from a `file` prop; its linked block outputs it) |

All image stages take `image` in and give `image` out unless they say otherwise. Pure stages
are cacheable; `save` and `background` are not. See `include/paperboat/toolkit/opencv.hpp`
and `src/` for the parameters and outputs of each block.

## Examples

`examples/` holds three complete backends. They are named `ocv-NN-<topic>`, so they are easy
to tell apart from the numbered tutorial curriculum:

| Example | Port | Shows |
| --- | --- | --- |
| `ocv-01-parts-counter` | 8781 | A synthetic conveyor, threshold/morphology, `contours` and `draw` |
| `ocv-02-shape-inspector` | 8782 | `hough_circles` and `contours` on one test card |
| `ocv-03-image-lab` | 8783 | `resize`, `crop`, `equalize`, `sobel` and `stats` |

The installer copies them into `engine/toolkit/opencv/examples/`; the CLI lists and starts
them like any other backend:

```sh
paperboat toolkit install opencv
paperboat backends
paperboat run ocv-01
```

## Using it in your own backend

Overlay this repository onto a Paperboat checkout so the toolkit sits at
`engine/toolkit/opencv/` (the engine discovers it automatically from its `toolkit.toml`):

```sh
# from your paperboat checkout
git clone https://github.com/paperboatlab/paperboat-toolkit-opencv /tmp/pb-opencv
cp -r /tmp/pb-opencv/engine/toolkit/opencv engine/toolkit/opencv
```

Then configure with OpenCV available:

```sh
sudo apt-get install -y libopencv-dev   # or your platform's equivalent
cmake -S engine -B engine/build -G Ninja -DPAPERBOAT_BUILD_TESTS=ON
cmake --build engine/build
```

In your backend's `main`, register it next to the builtins:

```cpp
tk::Registry r = tk::builtins();
tk::opencv::register_opencv(r);
```

and import `"opencv"` in your `descriptor.toml`. A backend that lists the toolkit in its
`backend.toml` (`toolkits = ["opencv"]`) is checked for it by `paperboat check`.

## CI

The build job checks out the (private) paperboat engine. Add a repository secret
`PAPERBOAT_TOKEN` (a PAT with repo scope) for it to run; otherwise only the `validate` job runs.
The engine's `engine/tests/opencv_test.cpp` covers the stages with synthetic Mats.
