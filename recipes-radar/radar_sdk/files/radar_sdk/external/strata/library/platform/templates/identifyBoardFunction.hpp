/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/BoardDescriptor.hpp>
#include <platform/boards/BoardRemote.hpp>
#include <platform/templates/boardFactoryFunction.hpp>

#include <common/cpp11/memory.hpp>


template <typename DescriptorType, typename... Args>
inline std::unique_ptr<DescriptorType> identifyBoardFunction(uint16_t ids[2], const char *cName, BoardData::const_iterator begin, BoardData::const_iterator end, Args... args)
{
    const uint16_t &VID = ids[0], &PID = ids[1];

    for (auto it = begin; it < end; it++)
    {
        if ((it->vid == VID) && (it->pid == PID))
        {
            return std::make_unique<DescriptorType>(*it, cName, std::forward<Args>(args)...);
        }
    }

    const BoardData d = {VID, PID, BoardFactoryFunction<BoardRemote>};
    return std::make_unique<DescriptorType>(d, cName, std::forward<Args>(args)...);
}
