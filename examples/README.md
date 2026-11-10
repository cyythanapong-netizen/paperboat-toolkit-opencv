# OpenCV toolkit examples

Each folder is a complete Paperboat backend that uses the optional `opencv` package. They
are named `ocv-NN-<topic>`: the `ocv` prefix keeps them apart from the numbered tutorial
curriculum in `paperboat-examples`, and `paperboat run ocv-01` finds the first one.

| Example | Port | Shows |
| --- | --- | --- |
| `ocv-01-parts-counter` | 8781 | A synthetic conveyor, threshold/morphology, `contours` and `draw` |
| `ocv-02-shape-inspector` | 8782 | `hough_circles` and `contours` on one test card |
| `ocv-03-image-lab` | 8783 | `resize`, `crop`, `equalize`, `sobel` and `stats` |

Build and run one from a Paperboat checkout after `paperboat toolkit install opencv`:

```sh
paperboat run ocv-01
paperboat check engine/toolkit/opencv/examples/ocv-01-parts-counter
```

The examples are copied to `engine/toolkit/opencv/examples/` when the toolkit is installed,
and the CLI lists them with `paperboat backends`.
