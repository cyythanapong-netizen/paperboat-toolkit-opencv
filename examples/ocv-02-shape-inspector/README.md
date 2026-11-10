# ocv-02-shape-inspector

A synthetic test card with one filled circle and one filled rectangle. Two OpenCV analyses
run on the same frame:

```text
Test card → Gray → Median → Hough circles → Draw → Image view
                 └→ Threshold → Contours → Draw → Image view
```

- **Hough circles** finds the circle's centre and radius; **Draw** rings it.
- **Contours** counts the two white blobs and gives the largest one's box; **Draw** frames it.
- The readouts compare the detected counts with the card's truth.

The run evaluates once, so the golden result is deterministic.

```sh
paperboat run ocv-02
paperboat check engine/toolkit/opencv/examples/ocv-02-shape-inspector
```
