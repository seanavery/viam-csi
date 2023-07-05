cmake_minimum_required(VERSION 3.25)

project(viam-csi
        VERSION 0.0.1
        DESCRIPTION "CSI camera modular resource"
)

set(CMAKE_CXX_STANDARD 14) # setting C++ 14 standard
find_package(PkgConfig REQUIRED) # finding pkg-config is a helper tool

# finding cpp sdk
set(CMAKE_PREFIX_PATH "/home/viam/viam-cpp-sdk/build/install" ${CMAKE_PREFIX_PATH})
find_package(viam-cpp-sdk CONFIG REQUIRED)

# using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-base-1.0)

# including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
)

# linking GStreamer library directory
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)

# building target executable
add_executable(${PROJECT_NAME} main.cpp)

# linking Gstreamer library with target executable
target_link_libraries(${PROJECT_NAME} 
        ${GSTREAMER_LIBRARIES} 
        viam-cpp-sdk::viamsdk
        viam_rust_utils
)

# running clang-tidy linter
# find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# if(CLANG_TIDY_EXE)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()

# installing executable
install(TARGETS ${PROJECT_NAME} DESTINATION usr/local/bin)
