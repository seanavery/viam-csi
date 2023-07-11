#include <gtest/gtest.h>

#include <viam/sdk/components/camera/camera.hpp>

#include "../csi_camera.cpp"
#include "../constraints.h"

using namespace viam::sdk;

// Test that the camera can be created and destroyed
TEST(CSICamera, CreateEmpty) {
    gst_init(nullptr, nullptr);

    AttributeMap attrs = std::make_shared<std::unordered_map<std::string, std::shared_ptr<ProtoType>>>();
    attrs->insert(std::make_pair("debug", std::make_shared<ProtoType>(true)));

    CSICamera camera("test", attrs);

    EXPECT_EQ(camera.is_debug(), true);
    EXPECT_EQ(camera.get_width_px(), DEFAULT_INPUT_WIDTH);
    EXPECT_EQ(camera.get_height_px(), DEFAULT_INPUT_HEIGHT);
    EXPECT_EQ(camera.get_frame_rate(), DEFAULT_INPUT_FRAMERATE);
    EXPECT_EQ(camera.get_video_path(), DEFAULT_INPUT_SENSOR);
}
