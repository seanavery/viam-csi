# CMake
CWD := .
BUILD_DIR := $(cwd)/build
INSTALL_DIR := $(BUILD_DIR)/AppDir

# clang-format
CXX := clang++
CXXFLAGS := -std=c++14 -Wall -Wextra
SRC := main.cpp csi_camera.cpp constraints.h

# Docker
L4T_VERSION := 35.3.1

# Module
# Builds/installs module.
.PHONY: build
build:
	rm -rf $(BUILD_DIR) | true && \
	mkdir -p build && \
	cd build && \
	cmake -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) .. -G Ninja && \
	ninja -j $(shell nproc)

# Creates appimage cmake build.
package:
	cd etc && \
	appimage-builder --recipe viam-csi-module-aarch64.yml

# lint:
# 	$(MAKE) build && \
# 	clang-tidy -p ./build $(SRC) -- $(CXXFLAGS)

# Builds docker image with viam-cpp-sdk and viam-csi installed.
image:
	rm -rf build | true && \
	docker build -t viam-csi:$(L4T_VERSION) \
		--memory=16g \
		--build-arg TAG=$(L4T_VERSION) \
		-f ./etc/Dockerfile.jetson ./

# Runs docker image with shell.
docker-module:
	docker run \
		--device /dev/fuse \
		--cap-add SYS_ADMIN \
		-it viam-csi:$(L4T_VERSION)

# Copies binary and AppImage from container to host.
bin-module:
	rm -rf bin | true && \
	mkdir -p bin && \
	docker stop viam-csi-bin | true && \
	docker rm viam-csi-bin | true && \
	docker run -d -it --name viam-csi-bin viam-csi:$(L4T_VERSION) && \
	docker cp viam-csi-bin:/root/opt/src/viam-csi/etc/viam-csi-0.0.1-aarch64.AppImage ./bin && \
	docker cp viam-csi-bin:/root/opt/src/viam-csi/build/viam-csi ./bin && \
	docker stop viam-csi-bin

# SDK
.PHONY: build-sdk
build-sdk:
	cd viam-cpp-sdk && \
	mkdir -p build && \
	cd build && \
	cmake -DVIAMCPPSDK_USE_DYNAMIC_PROTOS=ON -DVIAMCPPSDK_OFFLINE_PROTO_GENERATION=ON .. -G Ninja && \
	ninja -j $(shell nproc) && \
	sudo ninja install -j $(shell nproc))

docker-sdk:
	docker build -t viam-cpp-sdk -f ./viam-cpp-sdk/etc/docker/Dockerfile.ubuntu.focal ./ && \
	docker run -it viam-cpp-sdk /bin/bash

# Tests
# Tests out package in a fresh container.
docker-tests:
	docker build \
		-t viam-csi-tests \
		--build-arg TAG=$(L4T_VERSION) \
		-f ./etc/Dockerfile.test.jetson ./ && \
	docker run  \
		--device /dev/fuse \
		--cap-add SYS_ADMIN \
		-it \
		viam-csi-tests \
		/bin/bash

docker-ci:
	docker buildx build \
		-f etc/Dockerfile.jetson \
		--platform linux/arm64 \
		-t viam-csi-ci:latest \
		--build-arg TAG=35.3.1 \
		./

# Utils
# Installs waveshare camera overrides.
waveshare:
	mkdir -p gen && \
	wget https://www.waveshare.com/w/upload/e/eb/Camera_overrides.tar.gz -O gen/Camera_overrides.tar.gz && \
	tar -xvf gen/Camera_overrides.tar.gz -C gen && \
	sudo cp gen/camera_overrides.isp /var/nvidia/nvcam/settings/ && \
	sudo chmod 664 /var/nvidia/nvcam/settings/camera_overrides.isp && \
	sudo chown root:root /var/nvidia/nvcam/settings/camera_overrides.isp
