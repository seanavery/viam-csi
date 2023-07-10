cmake_minimum_required(VERSION 3.25)

project(viam-csi
        VERSION 0.0.2
        DESCRIPTION "viam module for csi cameras"
)

set(CMAKE_CXX_STANDARD 14)

set(GNU_LIB_PATH /usr/lib/aarch64-linux-gnu)
set(GNU_INCLUDE_PATH /usr/include/aarch64-linux-gnu)
set(LOCAL_LIB_PATH /usr/local/lib)
set(LOCAL_INCLUDE_PATH /usr/local/include)

find_package(PkgConfig REQUIRED)
find_package(viam-cpp-sdk CONFIG REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-base-1.0)

include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
        ${GNU_INCLUDE_PATH}
        ${LOCAL_INCLUDE_PATH}
)

link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
        ${GNU_LIB_PATH}
        ${LOCAL_LIB_PATH}
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
        ${GSTREAMER_LIBRARIES} 
        viam-cpp-sdk::viamsdk
)

install(TARGETS ${PROJECT_NAME} DESTINATION usr/local/bin)
