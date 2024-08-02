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
 *   @file system_test.hpp
 *
 *   System test suite.
 *
 *****************************************************************************/
/**
 * @file system_test.hpp
 *
 * Implementation of system tests
 *
 * @defgroup system_test_group System tests for the zukimo platform.
 *
 * @brief   This test group contains test to check the hardware functions of
 *          the system.
 *
 * @{
 *****************************************************************************/
#ifndef __SYSTEM_TEST_HPP__
#define __SYSTEM_TEST_HPP__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

/** @brief The fixture for testing the system test. */
class SystemTest : public ::testing::Test
{
protected:
  SystemTest()
  {
  }

  virtual ~SystemTest()
  {
    // You can do clean-up work that doesn't throw exceptions here.
  }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

  virtual void SetUp()
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown()
  {}

  bool doesFileExist(const char * fileName)
  {
    std::ifstream infile(fileName);
    return infile.good();
  }

  std::string getStringFromCmd(const std::string cmd, bool full = false)
  {
    char line[128];
    std::string ret;
  
    FILE * fp = popen(cmd.c_str(), "r");
    if (fp != NULL) {
      if (!full) {
        if (fgets(line, sizeof(line) - 1, fp) != NULL) {
          ret = line;
          ret.erase(std::remove(ret.begin(), ret.end(), '\n'), ret.end());
        }
      } else {
        do {
          if (fgets(line, sizeof(line) - 1, fp) != NULL) {
            ret += line;
          }
        } while(!feof(fp));
      }
    }
    pclose(fp);
    return ret;
  }

};

/** @} */ // end of system_test_testgroup1

#endif // __SYSTEM_TEST_HPP__