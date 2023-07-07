# viam-csi
> csi camera viam module

### why?

Want to use CSI cameras with viam? This is the module for you. viam-csi is a [Viam Module](https://docs.viam.com/extend/modular-resources/) that includes a simple wrapper around [GStreamer](https://gstreamer.freedesktop.org/documentation/?gi-language=c) and an interface that satisfies a [Viam Camera Component](https://docs.viam.com/components/camera/webcam/). This means you can utilize the hardware accelerated GST plugins on your Jetson or Pi with the Viam ecosystem.

### usage

1. Download appimage from releases page.
```bash
sudo curl -o /usr/local/bin/viam-csi https://github.com/seanvery/viam-csi/archive/refs/tags/v0.0.01-viam-csi
sudo chmod a+rx /usr/local/bin/viam-csi
```

2. Run RDK with example config file [etc/app-config.json](https://github.com/seanavery/viam-csi/blob/master/etc/app-config.json).

### support
- [x] jetson
- [ ] pi
