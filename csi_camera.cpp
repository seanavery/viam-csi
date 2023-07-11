#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <viam/sdk/components/camera/camera.hpp>
#include <viam/api/component/camera/v1/camera.grpc.pb.h>

#include "constraints.h"
#include "utils.cpp"

using namespace viam::sdk;

class CSICamera : public Camera {
    private:
        // Camera attributes
        bool debug;
        int width_px = 0;
        int height_px = 0;
        int frame_rate = 0;
        std::string video_path;

        // GST attributes
        GstElement *pipeline = nullptr;
        GstBus *bus = nullptr;
        GstMessage *msg = nullptr;
        GstSample *sample = nullptr;
        GstBuffer *buffer = nullptr;
        GstElement* appsink = nullptr;

        // Pipeline
        std::string input_source;
        std::string output_encoder;
    
    public:
        explicit CSICamera(std::string name, AttributeMap attrs) : Camera(std::move(name)) {
            // Get device type
            device_type device = get_device_type();
            if (device == device_type::unknown) {
                std::cout << "ERROR: device type unknown" << std::endl;
                input_source = DEFAULT_INPUT_SOURCE;
                output_encoder = DEFAULT_OUTPUT_ENCODER;
            } else if (device == device_type::jetson) {
                input_source = JETSON_INPUT_SOURCE;
                output_encoder = JETSON_OUTPUT_ENCODER;
            } else if (device == device_type::pi) {
                input_source = PI_INPUT_SOURCE;
                output_encoder = PI_OUTPUT_ENCODER;
            }

            // Validate attributes
            validate_attrs(attrs);

            // Create GST pipeline
            std::string pipeline_args = create_pipeline();
            if (debug) {
                std::cout << "pipeline_args: " << pipeline_args << std::endl;
            }
            
            // Start GST pipeline
            init_csi(pipeline_args);
        }

        void validate_attrs(AttributeMap attrs) {
            if (attrs->count("width_px") == 1) {
                std::shared_ptr<ProtoType> width_proto = attrs->at("width_px");
                auto width_value = width_proto->proto_value();
                if (width_value.has_number_value()) {
                    int width_num = static_cast<int>(width_value.number_value());
                    width_px = width_num;
                }
            }
            if (!width_px) {
                std::cout << "ERROR: width_px attribute not found" << std::endl;
                std::cout << "Setting width_px to default value: " << DEFAULT_INPUT_WIDTH << std::endl;
                width_px = DEFAULT_INPUT_WIDTH;
            }
                
            if (attrs->count("height_px") == 1) {
                std::shared_ptr<ProtoType> height_proto = attrs->at("height_px");
                auto height_value = height_proto->proto_value();
                if (height_value.has_number_value()) {
                    int height_num = static_cast<int>(height_value.number_value());
                    height_px = height_num;
                }
            }
            if (!height_px) {
                std::cout << "ERROR: height_px attribute not found" << std::endl;
                std::cout << "Setting height_px to default value: " << DEFAULT_INPUT_HEIGHT << std::endl;
                height_px = DEFAULT_INPUT_HEIGHT;
            }
            
            if (attrs->count("frame_rate") == 1) {
                std::shared_ptr<ProtoType> frame_proto = attrs->at("frame_rate");
                auto frame_value = frame_proto->proto_value();
                if (frame_value.has_number_value()) {
                    int frame_num = static_cast<int>(frame_value.number_value());
                    frame_rate = frame_num;
                }
            }
            if (!frame_rate) {
                std::cout << "ERROR: frame_rate attribute not found" << std::endl;
                std::cout << "Setting frame_rate to default value: " << DEFAULT_INPUT_SENSOR<< std::endl;
                frame_rate = DEFAULT_INPUT_FRAMERATE;
            }
            
            if (attrs->count("video_path") == 1) {
                std::shared_ptr<ProtoType> video_proto = attrs->at("video_path");
                auto video_value = video_proto->proto_value();
                if (video_value.has_string_value()) {
                    std::string video_str = video_value.string_value();
                    video_path = video_str;
                }
            }
            if (video_path.empty() ) {
                std::cout << "ERROR: video_path attribute not found" << std::endl;
                std::cout << "Setting video_path to default value: " << DEFAULT_INPUT_SENSOR << std::endl;
                video_path = DEFAULT_INPUT_SENSOR;
            }

            if (attrs->count("debug") == 1) {
                std::shared_ptr<ProtoType> debug_proto = attrs->at("debug");
                auto debug_value = debug_proto->proto_value();
                if (debug_value.has_bool_value()) {
                    bool debug_bool = static_cast<bool>(debug_value.bool_value());
                    debug = debug_bool;
                }
            }
        }

        // OVERRIDE

        void reconfigure(Dependencies deps, ResourceConfig cfg) override {
            if (debug) {
                std::cout << "Reconfiguring CSI Camera module" << std::endl;
            }

            // Update attributes
            auto attrs = cfg.attributes();
            validate_attrs(attrs);

            // Stop gst pipeline
            stop_pipeline();

            // Create GST pipeline
            std::string pipeline_args = create_pipeline();
            if (debug) {
                std::cout << "pipeline_args: " << pipeline_args << std::endl;
            }

            // Start GST pipeline
            init_csi(pipeline_args);
        }

        raw_image get_image(std::string mime_type) override {
            if (debug) {
                std::cout << "hit get_image. expecting mime_type " << mime_type << std::endl;
            }
            raw_image image;
            image.mime_type = DEFAULT_OUTPUT_MIMETYPE;
            if (FAKE_CAMERA) {
                image.bytes = get_test_image();
                return image;
            } else {
                // TODO: handle no bytes retrieved
                image.bytes = get_csi_image();
            }

            return image;
        }

        API dynamic_api() const override {
            return Camera::static_api();
        }

        AttributeMap do_command(AttributeMap command) override {
            std::cerr << "do_command not implemented" << std::endl;
            return 0;
        }

        point_cloud get_point_cloud(std::string mime_type) override {
            std::cerr << "get_point_cloud not implemented" << std::endl;
            return point_cloud{};
        }
        std::vector<GeometryConfig> get_geometries() override {
            std::cerr << "get_geometries not implemented" << std::endl;
            return std::vector<GeometryConfig>{};
        }

        properties get_properties() override {
            std::cerr << "get_properties not implemented" << std::endl;
            return properties{};
        };

        // GST

        void init_csi(std::string pipeline_args) {
            // Build gst pipeline
            pipeline = gst_parse_launch(
                pipeline_args.c_str(),
                nullptr
            );
            if (!pipeline) {
                std::cerr << "Failed to create the pipeline" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // Print pipeline structure
            if (debug) {
                GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline_structure");
            }
            
            // Fetch the appsink element
            appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink0");
            if (!appsink) {
                std::cerr << "Failed to get the appsink element" << std::endl;
                gst_object_unref(pipeline);
                std::exit(EXIT_FAILURE);
            }

            // Start the pipeline
            if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
                std::cerr << "Failed to start the pipeline" << std::endl;
                gst_object_unref(appsink);
                gst_object_unref(pipeline);
                std::exit(EXIT_FAILURE);
            }

            // Handle async pipeline creation
            wait_pipeline();

            // Run the main loop
            bus = gst_element_get_bus(pipeline);
            if (!bus) {
                std::cerr << "Failed to get the bus for the pipeline" << std::endl;
                gst_object_unref(appsink);
                gst_object_unref(pipeline);
                std::exit(EXIT_FAILURE);
            }

            return;
        }

        // Handles async GST state change
        void wait_pipeline() {
            GstState state, pending;
            GstStateChangeReturn ret;
            while ((ret = gst_element_get_state(pipeline, &state, &pending, GST_CLOCK_TIME_NONE)) == GST_STATE_CHANGE_ASYNC) {
                // wait for state change to complete
            }

            if (ret == GST_STATE_CHANGE_SUCCESS) {
                std::cout << "GST pipeline state change success" << std::endl;
            } else if (ret == GST_STATE_CHANGE_FAILURE) {
                std::cerr << "GST pipeline failed to change state" << std::endl;
                std::exit(EXIT_FAILURE);
            } else if (ret = GST_STATE_CHANGE_NO_PREROLL) {
                std::cout << "GST pipeline changed but not enough data for preroll" << std::endl;
            } else {
                std::cerr << "GST pipeline failed to change state" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return;
        }
        
        void stop_pipeline() {
            if (debug) {
                std::cout << "Stopping GST pipeline" << std::endl;
            }

            // Stop the pipeline
            if (gst_element_set_state(pipeline, GST_STATE_NULL) == GST_STATE_CHANGE_FAILURE) {
                std::cerr << "Failed to stop the pipeline" << std::endl;
                gst_object_unref(appsink);
                gst_object_unref(pipeline);
                std::exit(EXIT_FAILURE);
            }

            // Wait for async state change
            wait_pipeline();

            // Free resources
            gst_object_unref(appsink);
            gst_object_unref(pipeline);
            gst_object_unref(bus);
            appsink = nullptr;
            pipeline = nullptr;
            bus = nullptr;

            return;
        }
        
        void catch_pipeline() {
            GError* error = nullptr;
            gchar* debugInfo = nullptr;

            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error(msg, &error, &debugInfo);
                    std::cerr << "Error: " << error->message << std::endl;
                    std::cerr << "Debug Info: " << debugInfo << std::endl;
                    stop_pipeline();
                    std::exit(EXIT_FAILURE);
                    break;
                case GST_MESSAGE_EOS:
                    std::cout << "End of stream received" << std::endl;
                    stop_pipeline();
                    std::exit(EXIT_SUCCESS);
                    break;
                case GST_MESSAGE_WARNING:
                    gst_message_parse_warning(msg, &error, &debugInfo);
                    if (debug) {
                        std::cout << "Warning: " << error->message << std::endl;
                        std::cout << "Debug Info: " << debugInfo << std::endl;
                    }
                    break;
                case GST_MESSAGE_INFO:
                    gst_message_parse_info(msg, &error, &debugInfo);
                    if (debug) {
                        std::cout << "Info: " << error->message << std::endl;
                        std::cout << "Debug Info: " << debugInfo << std::endl;
                    }
                    break;
                default:
                    // Ignore other message types
                    break;
            }

            // Cleanup
            if (error != nullptr)
                g_error_free(error);
            if (debugInfo != nullptr)
                g_free(debugInfo);
        }

        std::vector<unsigned char> get_csi_image() {
            // Pull sample from appsink
            std::vector<unsigned char> vec;
            sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
            if (sample != nullptr) {
                // Retrieve buffer from the sample
                buffer = gst_sample_get_buffer(sample);

                // Process or handle the buffer as needed
                vec = buff_to_vec(buffer);

                // Release the sample
                gst_sample_unref(sample);
            }

            // Check bus for messages
            msg = gst_bus_pop(bus);
            if (msg != nullptr) {
                catch_pipeline();
                gst_message_unref(msg);
                msg = nullptr;
            }

            return vec;
        }

        // UTILS 

        std::string create_pipeline() {
            std::ostringstream oss;

            oss << input_source << " sensor_id=" << video_path
                << " ! " << DEFAULT_INPUT_FORMAT
                << ",width=" << std::to_string(width_px)
                << ",height=" << std::to_string(height_px)
                << ",framerate=" << std::to_string(frame_rate)
                << "/1 ! nvvidconv flip-method=" << DEFAULT_INPUT_FLIP_METHOD
                << " ! " << DEFAULT_OUTPUT_FORMAT
                << ",width=" << std::to_string(DEFAULT_OUTPUT_WIDTH)
                << ",height=" << std::to_string(DEFAULT_OUTPUT_HEIGHT)
                << " ! " << output_encoder
                << " ! " << DEFAULT_OUTPUT_MIMETYPE
                << " ! appsink name=appsink0 max-buffers=1";

            return oss.str();
        }

        std::vector<unsigned char> buff_to_vec(GstBuffer *buff) {
            // Get the size of the buffer
            size_t bufferSize = gst_buffer_get_size(buffer);

            // Create a vector with the same size as the buffer
            std::vector<unsigned char> vec(bufferSize);

            // Copy the buffer data to the vector
            GstMapInfo map;
            gst_buffer_map(buffer, &map, GST_MAP_READ);
            memcpy(vec.data(), map.data, bufferSize);
            gst_buffer_unmap(buffer, &map);

            return vec;
        }
        
        std::vector<unsigned char> get_test_image() {
            std::string test_image_path = "./etc/viam-logo.jpeg";
            
            // Create filestream
            std::ifstream file(test_image_path, std::ios::binary);
            if (!file) {
                std::cout << "ERROR: could not open file" << std::endl;
                return {};
            }

            // Read the file contents into a vector
            std::vector<unsigned char> bytes(std::istreambuf_iterator<char>(file), {});

            return bytes;
        }

        // GETTERS

        std::string get_video_path() const {
            return video_path;
        }

        int get_width_px() const {
            return width_px;
        }

        int get_height_px() const {
            return height_px;
        }

        int get_frame_rate() const {
            return frame_rate;
        }

        bool is_debug() const {
            return debug;
        }

        GstElement* get_appsink() const {
            return appsink;
        }
        
        GstElement* get_pipeline() const {
            return pipeline;
        }

        GstBus* get_bus() const {
            return bus;
        }
};
