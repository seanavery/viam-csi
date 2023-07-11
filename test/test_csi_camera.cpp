#include <gtest/gtest.h>

#include <viam/sdk/components/camera/camera.hpp>

#include "../csi_camera.cpp"
#include "../constraints.h"

using namespace viam::sdk;

// Test that the camera can be created with default values
TEST(CSICamera, CreateDefault) {
    gst_init(nullptr, nullptr);

    AttributeMap attrs = std::make_shared<std::unordered_map<std::string, std::shared_ptr<ProtoType>>>();
    attrs->insert(std::make_pair("debug", std::make_shared<ProtoType>(true)));

    CSICamera camera("test", attrs);

    EXPECT_EQ(camera.is_debug(), true);
    EXPECT_EQ(camera.get_width_px(), DEFAULT_INPUT_WIDTH);
    EXPECT_EQ(camera.get_height_px(), DEFAULT_INPUT_HEIGHT);
    EXPECT_EQ(camera.get_frame_rate(), DEFAULT_INPUT_FRAMERATE);
    EXPECT_EQ(camera.get_video_path(), DEFAULT_INPUT_SENSOR);

    camera.stop_pipeline();
}

// Test that the camera can be created with custom values
TEST(CSICamera, CreateCustom) {
    gst_init(nullptr, nullptr);

    AttributeMap attrs = std::make_shared<std::unordered_map<std::string, std::shared_ptr<ProtoType>>>();
    attrs->insert(std::make_pair("debug", std::make_shared<ProtoType>(true)));
    attrs->insert(std::make_pair("width_px", std::make_shared<ProtoType>(640)));
    attrs->insert(std::make_pair("height_px", std::make_shared<ProtoType>(480)));
    attrs->insert(std::make_pair("frame_rate", std::make_shared<ProtoType>(60)));
    attrs->insert(std::make_pair("video_path", std::make_shared<ProtoType>(std::string("1"))));

    CSICamera camera("test", attrs);

    EXPECT_EQ(camera.is_debug(), true);
    EXPECT_EQ(camera.get_width_px(), 640);
    EXPECT_EQ(camera.get_height_px(), 480);
    EXPECT_EQ(camera.get_frame_rate(), 60);
    EXPECT_EQ(camera.get_video_path(), "1");

    camera.stop_pipeline();
}

// Test that GST pipeline can be started and stopped
TEST(CSICamera, StartStopPipeline) {
    gst_init(nullptr, nullptr);

    AttributeMap attrs = std::make_shared<std::unordered_map<std::string, std::shared_ptr<ProtoType>>>();
    attrs->insert(std::make_pair("debug", std::make_shared<ProtoType>(true)));

    CSICamera camera("test", attrs);

    auto pipeline = camera.get_pipeline();
    auto appsink = camera.get_appsink();
    EXPECT_EQ(GST_STATE(pipeline), GST_STATE_PLAYING);
    EXPECT_EQ(GST_STATE(appsink), GST_STATE_PLAYING);

    camera.stop_pipeline();
}
