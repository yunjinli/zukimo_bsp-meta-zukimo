/******************************************************************************
 *
 * Copyright 2024, Dream Chip Technologies GmbH. All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Dream Chip Technologies GmbH, Steinriede 10, 30827 Garbsen / Berenbostel,
 * Germany
 *
 *****************************************************************************/
/**
 *   @file system_test_test.cpp
 *
 *   System test suite.
 *
 *****************************************************************************/
/**
 * @file system_test_test.cpp
 *
 * Implementation of system tests
 *
 * @defgroup system_test_test_group System tests for the zukimo platform.
 *
 * @brief   This test group contains test to check the hardware functions of
 *          the system.
 *
 * @{
 *****************************************************************************/
#include <sstream>
#include "system_test.hpp"


/**************************************************************************//**
 *
 * @brief  Read BSP version.
 *
 * @test   Verify that the build is a release.
 *
 * @note   To execute the single test call:
 *         /usr/bin/system_test --gtest_filter=*release_version_valid
 *
 * @pre  Condition 1.
 *
 * @post Condition 1.
 *
 *****************************************************************************/
TEST_F(SystemTest, release_version_valid)
{
    ASSERT_TRUE(doesFileExist("/etc/firmware.ver")) <<
    "Missing version file" << std::endl;

    // read in the first line containing the version
    std::string cmd("cat /etc/firmware.ver");
    std::string result = getStringFromCmd(cmd);

    // test if the first line contains a valid release version
    // which is defined by containing only a version (0.0.0.0)
    // without any branch information
    int buf[4];
    int cnt = sscanf(result.c_str(), "%d.%d.%d.%d",
        &buf[0], &buf[1], &buf[2], &buf[3]);
    ASSERT_TRUE(cnt == 4) << "Build is no release" << std::endl;

    std::ifstream stream("/etc/firmware.ver");
    std::stringstream buffer;
    buffer << stream.rdbuf();
    result = buffer.str();

    ASSERT_TRUE(result.find("dirty") == std::string::npos) <<
    "Build is dirty" << std::endl;
}


/**************************************************************************//**
 *
 * @brief   Read version of build layers.
 *
 * @test    Verify that all build layers are checked in.
 *
 * @note    To execute the single test call:
 *          /usr/bin/system_test --gtest_filter=*build_layers_valid
 *
 * @pre  Condition 1.
 *
 * @post Condition 1.
 *
 *****************************************************************************/
TEST_F(SystemTest, build_layers_valid)
{
    ASSERT_TRUE(doesFileExist("/etc/build-layers-git-revisions")) <<
    "Missing version file" << std::endl;

    std::ifstream stream("/etc/build-layers-git-revisions");
    std::stringstream buffer;
    buffer << stream.rdbuf();
    std::string result = buffer.str();

    ASSERT_TRUE(result.find("dirty") == std::string::npos) <<
    "Build is dirty" << std::endl;
    ASSERT_TRUE(result.find("modified") == std::string::npos) <<
    "Build is dirty" << std::endl;
}


/**************************************************************************//**
 *
 * @brief   PCIE is available
 *
 * @test    The system is able to enumerate the pci bus and finds its bridge.
 *
 * @note    To execute the single test call:
 *          /usr/bin/system_test --gtest_filter=*pcie_available
 *
 * @pre  Condition 1.
 *
 * @post Condition 1.
 *
 *****************************************************************************/
TEST_F(SystemTest, pcie_available)
{
    const std::string cmd = R"(lspci)";
    const std::string result_exp = "00:00.0 PCI bridge: Synopsys, Inc. Device dc00 (rev 01)";
    std::string result = getStringFromCmd(cmd, true);
    EXPECT_TRUE(result.find(result_exp) != std::string::npos) <<
    "Unable to find pci device (lspci)" << std::endl;
}

/**************************************************************************//**
 *
 * @brief   V4l2 devices available
 *
 * @test    The system discovered all v4l2 devices correctly.
 *
 * @note    To execute the single test call:
 *          /usr/bin/system_test --gtest_filter=*video_devices_available
 *
 * @pre  Condition 1.
 *
 * @post Condition 1.
 *
 *****************************************************************************/
TEST_F(SystemTest, video_devices_available)
{
    EXPECT_TRUE(doesFileExist("/dev/video/isp0"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp0-dpc"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp0-hist"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp0-hv"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp0-in"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp0-mv"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1-dpc"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1-hist"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1-hv"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1-in"));
    EXPECT_TRUE(doesFileExist("/dev/video/isp1-mv"));
    EXPECT_TRUE(doesFileExist("/dev/video/mvdu0"));
    EXPECT_TRUE(doesFileExist("/dev/video/mvdu1"));
    EXPECT_TRUE(doesFileExist("/dev/video/mvdu2"));
    EXPECT_TRUE(doesFileExist("/dev/video/nna"));
    EXPECT_TRUE(doesFileExist("/dev/video/sensor0"));
    EXPECT_TRUE(doesFileExist("/dev/video/vi0"));
    EXPECT_TRUE(doesFileExist("/dev/video/vi1"));
    EXPECT_TRUE(doesFileExist("/dev/video/vi2"));
    EXPECT_TRUE(doesFileExist("/dev/video/vi3"));
}

/** @} */ // end of system_test_testgroup1