/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "EnumeratorLibUvc.hpp"

#include "BridgeV4l2.hpp"
#include <platform/BoardInstance.hpp>
#include <platform/BoardListProtocol.hpp>

#include <common/Logger.hpp>
#include <common/cpp11/memory.hpp>


bool BoardDescriptorLibUvc::isUsed()
{
    return false;
}

std::shared_ptr<IBridge> BoardDescriptorLibUvc::createBridge()
{
    return std::make_shared<BridgeV4l2>(std::move(m_devicePath));
}

EnumeratorLibUvc::EnumeratorLibUvc()
{
}

EnumeratorLibUvc::~EnumeratorLibUvc()
{
}

void EnumeratorLibUvc::enumerate(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    listener.onEnumerate(std::make_unique<BoardDescriptorLibUvc>(*BoardListProtocol::begin, "test", "/dev/video0"));
}
