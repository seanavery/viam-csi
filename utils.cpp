#include <iostream>
#include <string>

#define DEVICE_PATH "/proc/device-tree/model"

enum class device_type {
    unknown,
    jetson,
    pi
};

struct os_info {
    std::string os;
    std::string version;
    std::string kernel;
};

device_type get_device_type() {
    std::ifstream device_name(DEVICE_PATH);
    if (device_name.is_open()) {
        std::string line;
        while (std::getline(device_name, line)) {
            std::string lowercase_line = line;
            std::transform(lowercase_line.begin(), lowercase_line.end(), lowercase_line.begin(), ::tolower);
            // Check for specific terms in a case-insensitive manner
            if (lowercase_line.find("nvidia") != std::string::npos &&
                (lowercase_line.find("orin") != std::string::npos ||
                 lowercase_line.find("nano") != std::string::npos ||
                 lowercase_line.find("agx") != std::string::npos ||
                 lowercase_line.find("jetson") != std::string::npos)) {
                return device_type::jetson;
            } else if (lowercase_line.find("raspberry") != std::string::npos &&
                lowercase_line.find("pi") != std::string::npos) {
                return device_type::pi;
            }
        }
        device_name.close();
    }

    return device_type::unknown;
}

os_info get_os() {
    // TODO
    return os_info{};
}



