#pragma once

// Module
#define DEFAULT_SOCKET_PATH "/tmp/viam.csi.sock"
#define RESOURCE_TYPE "CSICamera"
#define API_NAMESPACE "viam"
#define API_TYPE "camera"
#define API_SUBTYPE "csi"

// Camera
#define FAKE_CAMERA false
#define DEFAULT_INPUT_SOURCE "nvarguscamerasrc"
#define DEFAULT_INPUT_SENSOR "0"
#define DEFAULT_INPUT_FORMAT "video/x-raw(memory:NVMM)"
#define DEFAULT_INPUT_WIDTH 1920
#define DEFAULT_INPUT_HEIGHT 1080
#define DEFAULT_INPUT_FRAMERATE 30
#define DEFAULT_INPUT_FLIP_METHOD "0"
#define DEFAULT_OUTPUT_FORMAT "video/x-raw"
#define DEFAULT_OUTPUT_WIDTH 960
#define DEFAULT_OUTPUT_HEIGHT 540
#define DEFAULT_OUTPUT_ENCODER "nvjpegenc"
#define DEFAULT_OUTPUT_MIMETYPE "image/jpeg"


#define JETSON_INPUT_SOURCE "nvarguscamerasrc"
#define JETSON_OUTPUT_ENCODER "nvjpegenc"

#define PI_INPUT_SOURCE "libcamerasrc"
#define PI_OUTPUT_ENCODER "jpegenc"
