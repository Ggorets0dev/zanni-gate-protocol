BUILD_DIR ?= build
CONFIG ?= Release

.PHONY: all config build test clean rebuild help

all: build

config:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CONFIG)

build: config
	cmake --build $(BUILD_DIR) --config $(CONFIG)

test: build
	ctest --test-dir $(BUILD_DIR) -C $(CONFIG) --output-on-failure --verbose

clean:
	cmake -E remove_directory $(BUILD_DIR)

rebuild: clean build

help:
	@echo Available targets:
	@echo   all     - Configure and build the project (default)
	@echo   config  - Run CMake configuration
	@echo   build   - Build the static library
	@echo   test    - Build and run unit tests
	@echo   clean   - Remove build directory
	@echo   rebuild - Clean and rebuild the project

