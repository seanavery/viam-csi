# viam-csi
> viam module for csi cams

![](./etc/viam-server.png)


> **Warning** Currently in ALPHA and only supports Jetson boards. Please open an issue if you run into problems.

___

## Purpose

Want to use CSI cameras with viam? This is the module for you.

Most embedded cameras do not have an onboard [ISP](https://en.wikipedia.org/wiki/Image_processor) (image signal processor) and instead rely on the host machine for things like debayering, gamma-curve adjustments, gain control, white balance, and color correction.

ISPs are hard, so why roll your own? There are great libraries out there like [libargus](https://docs.nvidia.com/jetson/l4t-multimedia/group__LibargusAPI.html) and [libcamera](https://github.com/kbingham/libcamera).

`viam-csi` is a [Viam Module](https://docs.viam.com/extend/modular-resources/) that includes a simple wrapper around [GStreamer](https://gstreamer.freedesktop.org/documentation/?gi-language=c) and an interface that satisfies a [Viam Camera Component](https://docs.viam.com/components/camera/webcam/). This means you can utilize the hardware accelerated GST plugins on your Jetson or Pi with the Viam ecosystem.

___

## Usage

To use this module, follow these instructions to [add a module from the Viam Registry](https://docs.viam.com/registry/configure/#add-a-modular-resource-from-the-viam-registry) and select the `viam:camera:csi` model from the [`csi-cam` module](https://app.viam.com/module/viam/csi-cam).

## Configure your CSI camera

> [!NOTE]  
> Before configuring your camera, you must [create a robot](https://docs.viam.com/manage/fleet/robots/#add-a-new-robot).

Navigate to the **Config** tab of your robot’s page in [the Viam app](https://app.viam.com/). Click on the **Components** subtab and click **Create component**. Select the `camera` type, then select the `csi` model. Enter a name for your camera and click **Create**.

On the new component panel, copy and paste the following attribute template into your camera's **Attributes** box. 
```json
{
  "width_px": <int>,
  "height_px": <int>,
  "frame_rate": <int>,
  "debug": <bool>
}
```

> [!NOTE]  
> For more information, see [Configure a Robot](https://docs.viam.com/manage/configuration/).

Edit the attributes as applicable and save your config.
In the **Control** tab of the [Viam app](https://app.viam.com/), you can now view the camera feed. 
If you do not see anything, check the logs tab for errors.

### Attributes

The following attributes are available for `viam:camera:csi` cameras:

<!-- prettier-ignore -->
| Name | Type | Inclusion | Description |
| ---- | ---- | --------- | ----------- |
| `width_px` | int | Optional | Width of the image this camera captures in pixels. <br> Default: `1920` |
| `height_px` | int | Optional | Height of the image this camera captures in pixels. <br> Default: `1080` |
| `frame_rate` | int | Optional | The image capture frame rate this camera should use. <br> Default: `30` |
| `video_path` | string | Optional | The filepath to the input sensor of this camera on your board. If none is given, your robot will attempt to detect the video path automatically. <br> Default: `"0"` </br>  |
| `debug` | boolean | Optional | Whether or not you want debug input from this camera in your robot's logs. <br> Default: `false` |

### Example Configuration

```json
{
  "modules": [
    {
      "executable_path": "/usr/local/bin/viam-csi",
      "name": "csi_cam_module"
    }
  ],
  "components": [
    {
      "model": "viam:camera:csi",
      "attributes": {
        "width_px": 1920,
        "height_px": 1080,
        "frame_rate": 30,
        "debug": true
      },
      "depends_on": [],
      "name": "csicam_test",
      "namespace": "rdk",
      "type": "camera"
    }
  ]
}
```
___

### Support

Check [SUPPORT.md](./doc/SUPPORT.md) for a more comprehensive list of tested setups.
- [x] jetson
- [ ] pi
