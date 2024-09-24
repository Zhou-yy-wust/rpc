cmake_minimum_required(VERSION 3.22.1)
project(RPC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# 配置yaml-cpp
add_subdirectory(thirdParty/yaml-cpp)
set(Yaml_INCLUDE_DIRS thirdParty/yaml-cpp/include)

# 配置protobuf
set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED)
if(NOT Protobuf_FOUND)
    message(FATAL_ERROR "Protobuf not found! Make sure Protobuf is installed and accessible.")
else ()
    message(STATUS "Protobuf_VERSION: ${Protobuf_VERSION}")
    message(STATUS "Protobuf_INCLUDE_DIRS: ${Protobuf_INCLUDE_DIRS}")
    message(STATUS "Protobuf_LIBRARIES: ${Protobuf_LIBRARIES}")
    message(STATUS "Protobuf_PROTOC_EXECUTABLE: ${Protobuf_PROTOC_EXECUTABLE}")
endif ()

# 配置muduo
add_subdirectory(thirdParty/muduo)
set(muduo_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdParty/muduo)

# 配置zookeeper
set(ZK_INCLUDE_DIR /usr/local/include/zookeeper)
set(ZK_LIBRARIES /usr/local/lib/libzookeeper_mt.a)

#add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(example)
