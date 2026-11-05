# paperboat-toolkit-opencv

The optional image-processing toolkit for [Paperboat](https://github.com/cyythanapong-netizen/paperboat).

It provides the `opencv` package: image filters, an `Image` station object and an
`image` Dashboard instrument. It depends on OpenCV and is kept out of the core engine
so the main build stays light.

## Using it

Overlay this repository onto a Paperboat checkout so the toolkit sits at
`engine/toolkit/opencv/` (the engine discovers it automatically from its `toolkit.toml`):

```sh
# from your paperboat checkout
git clone https://github.com/cyythanapong-netizen/paperboat-toolkit-opencv /tmp/pb-opencv
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

and import `"opencv"` in your `descriptor.toml`.

## CI

The build job checks out the (private) paperboat engine. Add a repository secret
`PAPERBOAT_TOKEN` (a PAT with repo scope) for it to run; otherwise only the
`validate` job runs.
