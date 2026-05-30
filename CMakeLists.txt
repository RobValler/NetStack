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

#OPTIONS
option(TEST_ENABLE "Library Test Enable" OFF)

#PACKAGES
find_package(Threads REQUIRED)
#find_package(Boost 1.88.0 REQUIRED COMPONENTS boost)
#find_package(asio 1.36.0 REQUIRED)
find_package(protobuf 6.32.1 REQUIRED)
find_package(OpenSSL 3.6.1 REQUIRED)
find_package(libssh2 1.11.1 REQUIRED)
find_package(libssh 0.11.3 REQUIRED)
find_package(libxml2 2.15.0 REQUIRED)
find_package(logger 1.0.1 REQUIRED)


#INTERNAL INCLUDES
include_directories(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/encrypt)
include_directories(${CMAKE_SOURCE_DIR}/src/file)
include_directories(${CMAKE_SOURCE_DIR}/src/sftp)
include_directories(${CMAKE_SOURCE_DIR}/src/network)
include_directories(${CMAKE_SOURCE_DIR}/src/network/tcpip)
include_directories(${CMAKE_SOURCE_DIR}/src/network/udp)
include_directories(${CMAKE_SOURCE_DIR}/src/network/posix_mq)
include_directories(${CMAKE_SOURCE_DIR}/src/network/uds_ipc)
include_directories(${CMAKE_SOURCE_DIR}/src/network/helper)
include_directories(${CMAKE_SOURCE_DIR}/src/serialise)

#SOURCE FILES
file(GLOB APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp    
    ${CMAKE_SOURCE_DIR}/src/network/*.cpp
    ${CMAKE_SOURCE_DIR}/src/network/tcpip/*.cpp
    ${CMAKE_SOURCE_DIR}/src/network/udp/*.cpp
    ${CMAKE_SOURCE_DIR}/src/network/posix_mq/*.cpp
    ${CMAKE_SOURCE_DIR}/src/network/uds_ipc/*.cpp
    ${CMAKE_SOURCE_DIR}/src/network/helper/*.cpp
    ${CMAKE_SOURCE_DIR}/src/serialise/*.cpp
    ${CMAKE_SOURCE_DIR}/src/encrypt/*.cpp
    ${CMAKE_SOURCE_DIR}/src/file/*.cpp
    ${CMAKE_SOURCE_DIR}/src/sftp/*.cpp
)

#HEADERS FILES
file(GLOB_RECURSE APP_HEADERS
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/network/*.h
    ${CMAKE_SOURCE_DIR}/src/network/tcpip/*.h
    ${CMAKE_SOURCE_DIR}/src/network/udp/*.h
    ${CMAKE_SOURCE_DIR}/src/network/posix_mq/*.h
    ${CMAKE_SOURCE_DIR}/src/network/uds_ipc/*.h
    ${CMAKE_SOURCE_DIR}/src/network/helper/*.h
    ${CMAKE_SOURCE_DIR}/src/serialise/*.h
    ${CMAKE_SOURCE_DIR}/src/encrypt/*.h
    ${CMAKE_SOURCE_DIR}/src/file/*.h
    ${CMAKE_SOURCE_DIR}/src/sftp/*.h
)

#CONFIG FILES
file(GLOB APP_CONFIG
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/conanfile.py
    ${CMAKE_SOURCE_DIR}/create.sh
)

add_library(${PROJECT_NAME}
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_CONFIG}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    #Boost::boost
    #asio::asio
    protobuf::protobuf
    ssh::ssh
    libssh2::libssh2
    LibXml2::LibXml2
    OpenSSL::SSL
    logger::logger
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(TEST_ENABLE)
################### TEST ###################

#
find_package(GTest REQUIRED)

#CONFIG
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL test/messages)

#EXTERNAL INCLUDES
include_directories(${ProtobufIncludePath})

#INCLUDE
include_directories(${GTest_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/test/src)
include_directories(${CMAKE_SOURCE_DIR}/test/messages)

#SOURCE FILES
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/src/*.cpp
    ${CMAKE_SOURCE_DIR}/test/messages/*.cc
)

#HEADERS FILES
file(GLOB_RECURSE TEST_HEADERS
    ${CMAKE_SOURCE_DIR}/test/src/*.h
    ${CMAKE_SOURCE_DIR}/test/messages/*.pb.h
)

#PROTOBUF FILES
file(GLOB ProtoFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/test/messages/*.proto)

#GENERATE
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

add_executable(${PROJECT_NAME}_TEST
    ${TEST_SOURCES}
    ${TEST_HEADERS}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${ProtoFiles}    
)

target_link_libraries(${PROJECT_NAME}_TEST PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    #Boost::boost
    #asio::asio
    gtest::gtest
    protobuf::protobuf
    logger::logger
    net_stack
)
endif()

message("######################################################################")
message("# CONFIGURATION #")
message("#---------------#")
message("# Protobuf version:        [${protobuf_VERSION}]")
message("# OpenSSL version:         [${OpenSSL_VERSION}]")
message("# LibSSH version:          [${libssh_VERSION}]")
message("# LibXML2 version:         [${libxml2_VERSION}]")
message("# Logger version:          [${logger_VERSION}]")
message("# TEST_ENABLE:             [${TEST_ENABLE}]")
if(TEST_ENABLE)
message("# GTest version:           [${GTest_VERSION}]")
endif()
message("######################################################################")
