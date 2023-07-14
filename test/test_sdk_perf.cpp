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

    std::shared_ptr<vs::RobotClient> robot;
    try {
        robot = vs::RobotClient::at_address(robot_address, options);
        std::cout << "Successfully connected to the robot" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to connect to the robot. Exiting." << std::endl;
        throw;
    }


    // Get camera resource
    std::vector<vs::ResourceName>* resource_names = robot->resource_names();
    std::cout << "Resources of the robot:" << std::endl;
    for (vs::ResourceName resource : *resource_names) {
        std::cout << " - " << resource.name() << " (" << resource.subtype() << ")" << std::endl;
    }

    return EXIT_SUCCESS;
}