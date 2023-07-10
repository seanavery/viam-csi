#include <gtest/gtest.h>

#include "../csi_camera.cpp"
#include "../constraints.h"

// Test that the camera can be created and destroyed
TEST(CSICamera, CreateEmpty) {
    CSICamera camera("test", {});
}
