/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "serialise.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

CSerial::CSerial() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

CSerial::~CSerial() {
    google::protobuf::ShutdownProtobufLibrary();
}

bool CSerial::Serialise(const google::protobuf::Message& proto_message, std::vector<std::uint8_t>& outgoing_data, int& outgoing_size) {

    using namespace google::protobuf::io;

    // serialise data
    outgoing_size = proto_message.ByteSizeLong();
    outgoing_size += CodedOutputStream::VarintSize32(outgoing_size);
    outgoing_data.resize(outgoing_size);
    google::protobuf::io::ArrayOutputStream aos(&outgoing_data[0], outgoing_size);
    CodedOutputStream coded_output(&aos);
    coded_output.WriteVarint32(proto_message.ByteSizeLong());

    if(!proto_message.SerializeToCodedStream(&coded_output)) {
        return false;
    }

    return true;
}

void CSerial::Deserialise(const std::vector<std::uint8_t>& incomming_data, google::protobuf::Message& proto_message, int& outgoing_size) {

    using namespace google::protobuf::io;

    // convert from serialised char array to protobuf message class
    google::protobuf::io::ArrayInputStream ais(&incomming_data[0], outgoing_size);
    CodedInputStream coded_input(&ais);
    std::uint32_t size = static_cast<std::uint32_t>(outgoing_size);
    coded_input.ReadVarint32(&size);
    outgoing_size = size;
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(size);
    proto_message.ParseFromCodedStream(&coded_input);
    coded_input.ConsumedEntireMessage();
    coded_input.PopLimit(msgLimit);
}
