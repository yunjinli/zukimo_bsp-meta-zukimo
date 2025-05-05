/**
 * @copyright 2018 - 2021 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

/*
 * It seems that this file is not really required. It is referenced by
 * library/platform/CMakeLists.txt and CMake fails it is not present. On the
 * the other hand it seems that this header is not included at all. There is no
 * error if this files contains invalid C code.
 *
 * library/platform/CMakeLists.txt should be reviewed regarding the necessity
 * of this header. Also the according file for the Linux implementation seems
 * not to be good for anything because it includes a non-existing header file.
 */