# Distributed RPC Network Framework

## Introduction

This project is a distributed RPC network framework designed to provide efficient and scalable remote procedure call functionality. The framework utilizes several popular open-source libraries to support various features.

## Dependencies

This project depends on the following libraries:

- **protobuf**: For data serialization.
- **zookeeper**: Provides distributed coordination services.
- **yaml-cpp**: For parsing YAML configuration files.
- **muduo**: A high-performance C++ networking library.

Note: The `muduo` and `yaml-cpp` libraries are included in the `thirdParty` directory.

## Compilation

Before compiling the project, please ensure that you have installed all the dependencies. Then, follow these steps to compile:

1. Modify line 32 of the `CMakeLists.txt` file to set the include directory and link library for `zookeeper`:
    ```cmake
    set(ZK_INCLUDE_DIR /usr/local/include/zookeeper)
    set(ZK_LIBRARIES /usr/local/lib/libzookeeper_mt.a)
    ```

2. Use the following command to compile:
    ```shell
    cmake -B build && cmake --build build
    ```


The generated static libraries are located in the `lib` folder and include:

- **rbc**: The main library.
- **rbc_base**: Primarily an asynchronous logging library.

## Running Examples

Once compiled, you can start the server and client using the following commands:

- Start the server:
    ```shell
    ./bin/provider -i ./bin/test.yaml
    ```

- Start the client:
    ```shell
    ./bin/consumer -i ./bin/test.yaml
    ```

## Configuration

Please ensure that the parameters for the server and client are correctly configured in the `./bin/test.yaml` file so that they can connect and communicate properly.
