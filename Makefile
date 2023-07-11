# CMake
BUILD_DIR := ./build
INSTALL_DIR := $(BUILD_DIR)/AppDir
BIN_DIR := ./bin

# Docker
IMAGE_NAME := viam-csi
IMAGE_TAG := 0.0.2
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
	appimage-builder --recipe viam-csi-jetson-arm64.yml

pi-package:
	cd etc && \
	appimage-builder --recipe viam-csi-pi-arm64.yml

# Builds docker image with viam-cpp-sdk and viam-csi installed.
image:
	rm -rf build | true && \
	docker build -t $(IMAGE_NAME):$(IMAGE_TAG) \
		--memory=16g \
		--build-arg TAG=$(L4T_VERSION) \
		-f ./etc/Dockerfile.jetson ./

pi-image:
	docker build -t $(IMAGE_NAME)-pi:$(IMAGE_TAG) \
		--memory=16g \
		-f ./etc/Dockerfile.pi ./

# Runs docker image with shell.
docker-module:
	docker run \
		--device /dev/fuse \
		--cap-add SYS_ADMIN \
		-it $(IMAGE_NAME):$(IMAGE_TAG)

# Copies binary and appimage from container to host.
bin-module:
	rm -rf $(BIN_DIR) | true && \
	mkdir -p $(BIN_DIR) && \
	docker stop viam-csi-bin | true && \
	docker rm viam-csi-bin | true && \
	docker run -d -it --name viam-csi-bin $(IMAGE_NAME):$(IMAGE_TAG) && \
	docker cp viam-csi-bin:/root/opt/src/viam-csi/etc/viam-csi-$(IMAGE_TAG)-aarch64.AppImage $(BIN_DIR) && \
	docker cp viam-csi-bin:/root/opt/src/viam-csi/build/viam-csi $(BIN_DIR) && \
	docker stop viam-csi-bin

# SDK
.PHONY: build-sdk
build-sdk:
	cd viam-cpp-sdk && \
	mkdir -p build && \
	cd build && \
	cmake -DVIAMCPPSDK_USE_DYNAMIC_PROTOS=ON -DVIAMCPPSDK_OFFLINE_PROTO_GENERATION=ON .. -G Ninja && \
	ninja -j $(shell nproc) && \
	sudo ninja install -j $(shell nproc)) && \
	cp -r ./install/* /usr/local/

docker-sdk:
	docker build -t viam-cpp-sdk -f ./viam-cpp-sdk/etc/docker/Dockerfile.ubuntu.focal ./ && \
	docker run -it viam-cpp-sdk /bin/bash

# Tests
# Tests out package in a fresh container.
docker-tests:
	docker build \
		-t viam-csi-tests:$(IMAGE_TAG) \
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
# Installs waveshare camera overrides on Jetson.
waveshare:
	mkdir -p gen && \
	wget https://www.waveshare.com/w/upload/e/eb/Camera_overrides.tar.gz -O gen/Camera_overrides.tar.gz && \
	tar -xvf gen/Camera_overrides.tar.gz -C gen && \
	sudo cp gen/camera_overrides.isp /var/nvidia/nvcam/settings/ && \
	sudo chmod 664 /var/nvidia/nvcam/settings/camera_overrides.isp && \
	sudo chown root:root /var/nvidia/nvcam/settings/camera_overrides.isp

# Installs Arducam IMX477 driver on Jetson.
arducam:
	mkdir -p gen && \
	cd gen && \
	wget https://github.com/ArduCAM/MIPI_Camera/releases/download/v0.0.3/install_full.sh && \
	chmod +x install_full.sh && \
	./install_full.sh -m imx477
