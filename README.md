# mpv-socks
Synchronize multiple instances of mpv to remotely watch videos together.

![](https://thumbs.gfycat.com/InsistentShyHyena-size_restricted.gif)

## Usage

### Build the project:
```bash
meson build
ninja -C build
```

### Start the server:
```bash
./build/subprojects/socks/example_forward
```

### Start two mpv instances **in different directories**:
```bash
/path-to-project/build/mpv-controller 127.0.0.1 /path/to/video.mp4
```

Playing/pausing/seeking in one instance will now synchronize to other instances.
