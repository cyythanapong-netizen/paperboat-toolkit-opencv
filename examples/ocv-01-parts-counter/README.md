# ocv-01-parts-counter

A synthetic overhead conveyor with three bright parts and one speck of dust. The Flow page
is a classic OpenCV pipeline, built from the optional `opencv` package:

```text
Conveyor → Gray → Threshold → Open → Contours → Draw → Image view
                                       └→ count readout
```

- **Conveyor** (domain source) — a deterministic 320×240 frame: three white rectangles and
  a two-pixel speck.
- **Threshold → Open** — a black-and-white mask, then a 3×3 opening that removes the speck.
- **Contours** — counts the remaining white blobs, and gives the largest one's centre, area
  and bounding box.
- **Draw** — puts a green box around the largest part. `Contours` already marks every blob,
  so the Image view shows all three and the box calls out the largest.

The run evaluates once, so it is deterministic and needs no clock.

```sh
paperboat run ocv-01
paperboat check engine/toolkit/opencv/examples/ocv-01-parts-counter
```
