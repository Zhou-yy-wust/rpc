aux_source_directory(. SOURCES)
add_library(rpc STATIC ${SOURCES}
        zkclient.h
        zkclient.cpp)
target_include_directories(
        rpc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../base
        ${Yaml_INCLUDE_DIRS}
        ${muduo_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${ZK_INCLUDE_DIR}
)
target_link_libraries(rpc PUBLIC yaml-cpp muduo_base muduo_net rpc_base ${ZK_LIBRARIES})