# viam-csi
> viam module for csi cams

![](./etc/viam-server.png)


> **Warning** Currently in ALPHA and only supports Jetson. Please open an issue if you run into problems.

___

### why?

Want to use CSI cameras with viam? This is the module for you.

Most embedded cameras do not have an onboard [ISP](https://en.wikipedia.org/wiki/Image_processor) (image signal processor) and instead rely on the host machine for things like debayering, gamma-curve adjustments, gain control, white balance, and color correction.

ISPs are hard, so why roll your own? There are great libraries out there like [libargus](https://docs.nvidia.com/jetson/l4t-multimedia/group__LibargusAPI.html) and [libcamera](https://github.com/kbingham/libcamera).

`viam-csi` is a [Viam Module](https://docs.viam.com/extend/modular-resources/) that includes a simple wrapper around [GStreamer](https://gstreamer.freedesktop.org/documentation/?gi-language=c) and an interface that satisfies a [Viam Camera Component](https://docs.viam.com/components/camera/webcam/). This means you can utilize the hardware accelerated GST plugins on your Jetson or Pi with the Viam ecosystem.

___

### usage

1. Download appimage from releases page.
```bash
sudo wget https://github.com/seanavery/viam-csi/releases/download/v0.0.2/viam-csi-0.0.2-aarch64.AppImage -O /usr/local/bin/viam-csi
```

```bash
sudo chmod 755 /usr/local/bin/viam-csi
```

2. Run [viam-server](https://docs.viam.com/installation/) with example config file [etc/app-config.json](https://github.com/seanavery/viam-csi/blob/master/etc/app-config.json).

`viam-server` will automatically load the module. You can now head over to https://app.viam.com/ and view the control tab for the camera feed. If you do not see anything, check the logs tab for errors.

___

### support

Check [SUPPORT.md](./doc/SUPPORT.md) for a more comprehensive list of tested setups.
- [x] jetson
- [ ] pi
