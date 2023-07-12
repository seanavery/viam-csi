gst-launch-1.0 libcamerasrc ! 'video/x-raw,width=1920,height=1080,framerate=30/1' ! videoconvert ! jpegenc ! image/jpeg ! appsink name=appsink0 max-buffers=1
