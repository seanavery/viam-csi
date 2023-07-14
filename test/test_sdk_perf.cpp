#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <viam/sdk/components/camera/camera.hpp>
#include <viam/sdk/components/camera/client.hpp>
#include <viam/sdk/robot/client.hpp>
#include <viam/sdk/robot/service.hpp>
#include <viam/sdk/rpc/dial.hpp>

namespace vs = ::viam::sdk;

int main() {
    // Connect to robot
    std::string robot_address("localhost:8080");
    vs::Credentials credentials("");
    vs::DialOptions dial_options;
    dial_options.set_allow_insecure_downgrade(credentials.payload().empty());

    vs::Options options = vs::Options(1, dial_options);

    // Get camera resource
    return EXIT_SUCCESS;
}