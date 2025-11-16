#/*****************************************************************
# * Copyright (C) 2017 Robert Valler - All rights reserved.
# *
# * This file is part of the project: <insert project name here>
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.22.0)
project ( net_stack )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Wall)
set(W1)
set(Wfloat-conversion)

#OPTIONS
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL src/messages)

#PACKAGES
find_package(Threads REQUIRED)
find_package(Boost 1.88.0 REQUIRED COMPONENTS boost)
find_package(asio 1.36.0 REQUIRED)
find_package(protobuf 6.32.1 REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenSSL 3.6.0 REQUIRED)

#EXTERNAL INCLUDES
include_directories(${ProtobufIncludePath})

#INTERNAL INCLUDES
include_directories(${CMAKE_SOURCE_DIR}/test/main)
include_directories(${CMAKE_SOURCE_DIR}/messages)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/encrypt)
include_directories(${CMAKE_SOURCE_DIR}/src/file)
include_directories(${CMAKE_SOURCE_DIR}/src/network)
include_directories(${CMAKE_SOURCE_DIR}/src/network/tcpip)
include_directories(${CMAKE_SOURCE_DIR}/src/network/udp)
include_directories(${CMAKE_SOURCE_DIR}/src/network/posix)
include_directories(${CMAKE_SOURCE_DIR}/src/serialise)

#SOURCE FILES
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cc
)

#HEADERS FILES
file(GLOB_RECURSE APP_HEADERS
    ${CMAKE_SOURCE_DIR}/src/*.h
)

#PROTOBUF FILES
file(GLOB ProtoFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/messages/*.proto)

#CONFIG FILES
file(GLOB APP_CONFIG
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/conanfile.py
    ${CMAKE_SOURCE_DIR}/create.sh
)

#GENERATE
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

add_library(${PROJECT_NAME}
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${ProtoFiles}   
    ${APP_CONFIG}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    Boost::boost
    asio::asio
    protobuf::protobuf
    openssl::openssl
)


#if(TEST_ENABLE)
################### TEST ###################
#INCLUDE
include_directories(${GTest_INCLUDE_DIRS})

#SOURCE FILES
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/main.cpp
    ${CMAKE_SOURCE_DIR}/test/gtest_server.cpp
#    ${CMAKE_SOURCE_DIR}/messages/*.pb.cc
)

#HEADERS FILES
file(GLOB_RECURSE TEST_HEADERS
    ${CMAKE_SOURCE_DIR}/messages/*.pb.h
)

add_executable(${PROJECT_NAME}_TEST
    ${TEST_SOURCES}
    ${TEST_HEADERS}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_link_libraries(${PROJECT_NAME}_TEST PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    Boost::boost
    asio::asio
    gtest::gtest
    protobuf::protobuf
    net_stack
)
#endif()

message("######################################################################")
message("# CONFIGURATION #")
message("#################")
message("# ProtobufIncludePath:         [${ProtobufIncludePath}]")
message("######################################################################")
