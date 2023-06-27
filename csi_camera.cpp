#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

#include "constraints.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <viam/sdk/components/camera/camera.hpp>
#include <viam/api/component/camera/v1/camera.grpc.pb.h>

using namespace viam::sdk;

class CSICamera : public Camera {
    private:
        // Camera attributes
        std::string video_path;
        int width_px;
        int height_px;
        int frame_rate;
        bool debug;

        // Gstreamer attributes
        GstElement *pipeline = nullptr;
        GstBus *bus = nullptr;
        GstMessage *msg = nullptr;
        GstSample *sample = nullptr;
        GstBuffer *buffer = nullptr;
        GstElement* appsink = nullptr;
    
    public:
        explicit CSICamera(std::string name, AttributeMap attrs) : Camera(std::move(name)) {
            // Validate attributes
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
                std::cout << "Setting video_path to default value: " << DEFAULT_INPUT_SOURCE << std::endl;
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

            // Create GST pipeline
            std::string pipeline_args = create_pipeline();
            if (debug) {
                std::cout << "pipeline_args: " << pipeline_args << std::endl;
            }

            // Start GST pipeline
            csi_init(pipeline_args);
        }

        // OVERRIDE
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
                image.bytes = csi_get_image();
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
        void csi_init(std::string pipeline_args) {
            // Build gst pipeline
            pipeline = gst_parse_launch(
                pipeline_args.c_str(),
                nullptr
            );
            if (!pipeline) {
                std::cerr << "Failed to create the pipeline" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            
            // Fetch the appsink element
            appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink0");
            if (!appsink) {
                std::cerr << "Failed to get the appsink element" << std::endl;
                gst_object_unref(pipeline);
                std::exit(EXIT_FAILURE);
            }

            // Set the appsink to emit signal
            g_object_set(appsink, "emit-signals", TRUE, NULL);

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

            // Handle pipeline errors

            return;
        }

        // Handles errors from async GST state change
        void wait_pipeline() {
            GstState state, pending;
            GstStateChangeReturn ret;
            // while (gst_element_get_state(pipeline, &state, &pending, GST_CLOCK_TIME_NONE) == GST_STATE_CHANGE_ASYNC) {
            while ((ret = gst_element_get_state(pipeline, &state, &pending, GST_CLOCK_TIME_NONE)) == GST_STATE_CHANGE_ASYNC) {
                // wait for state change to complete
            }

            if (ret == GST_STATE_CHANGE_SUCCESS) {
                std::cout << "GST pipeline started" << std::endl;
            } else if (ret == GST_STATE_CHANGE_FAILURE) {
                std::cerr << "GST pipeline failed to start" << std::endl;
                std::exit(EXIT_FAILURE);
            } else if (ret = GST_STATE_CHANGE_NO_PREROLL) {
                std::cout << "GST pipeline started but not enough data for preroll" << std::endl;
            } else {
                std::cerr << "GST pipeline failed to start" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            return;
        }

        std::vector<unsigned char> csi_get_image() {
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

            // Check for EOS message
            msg = gst_bus_pop_filtered(bus, GST_MESSAGE_EOS);
            if (msg != nullptr) {
                std::cout << "end of stream received" << std::endl;
                gst_message_unref(msg);
                // Exit process when EOS message is received
                std::exit(EXIT_SUCCESS);
            }

            return vec;
        }

        // UTILS 
        std::string create_pipeline() {
            std::ostringstream oss;

            oss << DEFAULT_INPUT_SOURCE << " sensor_id=" << video_path
                << " ! " << DEFAULT_INPUT_FORMAT
                << ",width=" << std::to_string(width_px)
                << ",height=" << std::to_string(height_px)
                << ",framerate=" << std::to_string(frame_rate)
                << "/1 ! nvvidconv flip-method=" << DEFAULT_INPUT_FLIP_METHOD
                << " ! " << DEFAULT_OUTPUT_FORMAT
                << ",width=" << std::to_string(DEFAULT_OUTPUT_WIDTH)
                << ",height=" << std::to_string(DEFAULT_OUTPUT_HEIGHT)
                << " ! " << DEFAULT_OUTPUT_ENCODER
                << " ! " << DEFAULT_OUTPUT_MIMETYPE
                << " ! appsink max-buffers=1";

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
            std::string test_image_path = "/home/viam/csi-mr/etc/viam-logo.jpeg";
            
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
};
