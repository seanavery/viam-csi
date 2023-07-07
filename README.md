# viam-csi
> csi camera viam module

![](./etc/viam-server.png)


Currently in ALPHA and only supports Jetson. Please open an issue if you run into problems.

### why?

Want to use CSI cameras with viam? This is the module for you. 

[Viam Module](https://docs.viam.com/extend/modular-resources/) that includes a simple wrapper around [GStreamer](https://gstreamer.freedesktop.org/documentation/?gi-language=c) and an interface that satisfies a [Viam Camera Component](https://docs.viam.com/components/camera/webcam/). This means you can utilize the hardware accelerated GST plugins on your Jetson or Pi with the Viam ecosystem.

### usage

1. Download appimage from releases page.
```bash
sudo wget https://github.com/seanavery/viam-csi/releases/download/v0.0.1/viam-csi-0.0.1-aarch64.AppImage -O /usr/local/bin/viam-csi
sudo chmod a+rx /usr/local/bin/viam-csi
```

2. Run RDK with example config file [etc/app-config.json](https://github.com/seanavery/viam-csi/blob/master/etc/app-config.json).

### support
- [x] jetson
- [ ] pi
