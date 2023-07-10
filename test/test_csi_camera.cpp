#include <gtest/gtest.h>

#include "../csi_camera.cpp"
#include "../constraints.h"

// Test that the camera can be created and destroyed
TEST(CSICamera, CreateEmpty) {
    CSICamera camera("test", {});

    EXPECT_EQ(camera.is_debug(), false);
    EXPECT_EQ(camera.get_width_px(), DEFAULT_INPUT_WIDTH);
    EXPECT_EQ(camera.get_height_px(), DEFAULT_INPUT_HEIGHT);
    EXPECT_EQ(camera.get_frame_rate(), DEFAULT_INPUT_FRAMERATE);
    EXPECT_EQ(camera.get_video_path(), DEFAULT_INPUT_SENSOR);
}
