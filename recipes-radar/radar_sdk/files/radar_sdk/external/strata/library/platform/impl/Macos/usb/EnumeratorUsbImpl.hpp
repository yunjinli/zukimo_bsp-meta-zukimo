/**
 * @copyright 2018 - 2021 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/interfaces/IEnumerator.hpp>


/*
 * USB protocol is not implemented for MacOS yet.
 * This is just an empty dummy implementation of the USB enumerator, because
 * an implementation is needed to make the project link.
 */
class EnumeratorUsbImpl :
    public IEnumerator
{
public:
    EnumeratorUsbImpl()  = default;
    ~EnumeratorUsbImpl() = default;

    void enumerate(IEnumerationListener & /*listener*/, BoardData::const_iterator /*begin*/, BoardData::const_iterator /*end*/) override
    {}
};
