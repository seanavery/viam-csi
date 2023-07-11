#include <iostream>
#include <gst/gst.h>

#include <viam/sdk/module/module.hpp>
#include <viam/sdk/module/service.hpp>
#include <viam/sdk/resource/resource.hpp>
#include <viam/sdk/registry/registry.hpp>
#include <viam/sdk/components/camera/server.hpp>

#include "constraints.h"
#include "csi_camera.cpp"

using namespace viam::sdk;

int serve(const std::string& socket_path) {
    // Signal handlers
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    // Model regsitration
    auto module_registration = std::make_shared<ModelRegistration>(
        ResourceType{RESOURCE_TYPE},
        Camera::static_api(),
        Model{API_NAMESPACE, API_TYPE, API_SUBTYPE},
        [](Dependencies, ResourceConfig resource_config) -> std::shared_ptr<Resource> {
            return std::make_shared<CSICamera>(resource_config.name(), resource_config.attributes());
        },
        [](ResourceConfig resource_config) -> std::vector<std::string> { return {}; });

    try {
        Registry::register_model(module_registration);
        std::cout << "registered model: " << API_NAMESPACE <<  ":" << API_TYPE << ":" << API_SUBTYPE << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "error registering model: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
        
    // Socket
    auto module_service = std::make_shared<ModuleService_>(socket_path);
    
    // GRPC Server
    auto server = std::make_shared<Server>();
    module_service->add_model_from_registry(
        server,
        module_registration->api(),
        module_registration->model());
    module_service->start(server);
    
    // Start server thread
    std::thread server_thread([&server, &sigset]() {
        server->start();
        int sig = 0;
        auto result = sigwait(&sigset, &sig);
        server->shutdown();
    });
    server->wait();
    server_thread.join();

    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    std::cout << "### STARTING VIAM CSI CAMERA MODULE" << std::endl;

    // Gstreamer initialization
    gst_init(&argc, &argv);
    
    std::string sock_path = (argc < 2) ? DEFAULT_SOCKET_PATH : argv[1];
    std::cout << "binary:" << argv[0] << std::endl;
    std::cout << "socket: " << sock_path << std::endl;
    
    return serve(sock_path);
}
