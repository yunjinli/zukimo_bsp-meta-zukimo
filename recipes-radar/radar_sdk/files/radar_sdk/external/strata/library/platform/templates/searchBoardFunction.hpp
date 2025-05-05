/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "identifyBoardFunction.hpp"
#include <platform/interfaces/IEnumerator.hpp>

#include <common/Logger.hpp>
#include <common/Serialization.hpp>
#include <platform/exception/EConnection.hpp>


inline BoardData::const_iterator findBoardData(uint16_t vid, uint16_t pid, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    BoardData::const_iterator it;
    for (it = begin; it < end; it++)
    {
        if ((it->vid == vid) && (it->pid == pid))
        {
            break;
        }
    }

    return it;
}

template <typename BridgeType, typename IdentifierType, typename DescriptorType = BoardDescriptor>
inline std::unique_ptr<DescriptorType> searchBoardFunctionBridge(IdentifierType &identifier, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    IBridgeControl::BoardInfo_t boardInfo;
    const char *cName = reinterpret_cast<char *>(&boardInfo[4]);
    uint16_t ids[2];

    try
    {
        std::unique_ptr<IBridge> bridge = std::make_unique<BridgeType>(identifier);

        bridge->getIBridgeControl()->getBoardInfo(boardInfo);
        serialToHost(boardInfo.data(), boardInfo.data() + sizeof(*ids) * 2, ids);

        return identifyBoardFunction<DescriptorType>(ids, cName, begin, end, std::move(bridge));
    }
    catch (const EException &e)
    {
        LOG(DEBUG) << "... handled " << e.what();
        throw EConnection("Board not found! (no response received)");
    }
}

template <typename EnumeratorType>
inline std::unique_ptr<BoardDescriptor> searchBoardFunctionEnumerator(uint16_t vid, uint16_t pid, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    auto it = findBoardData(vid, pid, begin, end);

    class ListenerClass : public IEnumerationListener
    {
    public:
        bool onEnumerate(std::unique_ptr<BoardDescriptor> &&descriptor) override
        {
            m_descriptor = std::move(descriptor);
            return true;
        }

        std::unique_ptr<BoardDescriptor> m_descriptor;
    } listener;

    if (it != end)
    {
        EnumeratorType().enumerate(listener, it, it + 1);
    }
    else
    {
        const BoardData d = {vid, pid, BoardFactoryFunction<BoardRemote>};
        EnumeratorType().enumerate(listener, &d, &d + 1);
    }

    return std::move(listener.m_descriptor);
}
