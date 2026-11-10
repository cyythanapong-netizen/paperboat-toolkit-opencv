# ocv-03-image-lab

A synthetic horizontal gradient with a bright square. It exercises the geometry, tone and
gradient stages of the optional `opencv` package, plus `stats`:

```text
Gradient card → Stats → mean/min/max readouts
             └→ Gray → Resize → Crop → Equalize → Sobel → Image view
```

- **Resize** halves the width (height follows), **Crop** keeps an 80×60 window, **Equalize**
  spreads the histogram and **Sobel** shows the horizontal derivative.
- **Stats** reads the grey mean, minimum and maximum; the readouts compare them with the
  card's truth.

The run evaluates once, so the golden result is deterministic.

```sh
paperboat run ocv-03
paperboat check engine/toolkit/opencv/examples/ocv-03-image-lab
```
