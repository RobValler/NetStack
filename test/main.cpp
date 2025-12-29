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
    //::testing::GTEST_FLAG(filter) = "posix_mq.basic";
    ::testing::GTEST_FLAG(filter) = "udp.basic";
    //::testing::GTEST_FLAG(filter) = "tcpip.basic";

    result = RUN_ALL_TESTS();
    return result;
}
