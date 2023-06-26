#pragma once

// Module
#define DEFAULT_SOCKET_PATH "/tmp/viam.csi.sock"
#define RESOURCE_TYPE "CSICamera"
#define API_NAMESPACE "viam"
#define API_TYPE "camera"
#define API_SUBTYPE "csi"

// Camera
#define FAKE_CAMERA false
#define STREAMER_INPUT_SOURCE "nvarguscamerasrc"
#define STREAMER_INPUT_SENSOR "0"
#define STREAMER_INPUT_FORMAT "video/x-raw(memory:NVMM)"
#define STREAMER_INPUT_WIDTH 1920
#define STREAMER_INPUT_HEIGHT 1080
#define STREAMER_INPUT_FRAMERATE 30
#define STREAMER_INPUT_FLIP_METHOD "0"
#define STREAMER_OUTPUT_FORMAT "video/x-raw"
#define STREAMER_OUTPUT_WIDTH 960
#define STREAMER_OUTPUT_HEIGHT 540
#define STREAMER_OUTPUT_ENCODER "nvjpegenc"
#define STREAMER_OUTPUT_MIMETYPE "image/jpeg"
