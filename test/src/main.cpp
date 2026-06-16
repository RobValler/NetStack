/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    int result;
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "*";
    //::testing::GTEST_FLAG(filter) = "encrypt.AES";
    //::testing::GTEST_FLAG(filter) = "network.connect";
    //::testing::GTEST_FLAG(filter) = "encrypt.TLS";
    //::testing::GTEST_FLAG(filter) = "network.json_test";
    //::testing::GTEST_FLAG(filter) = "unix_sock.get";
    ::testing::GTEST_FLAG(filter) = "unix_sock.json_test";

    result = RUN_ALL_TESTS();
    return result;
}
