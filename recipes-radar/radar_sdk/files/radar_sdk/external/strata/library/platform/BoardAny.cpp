/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BoardAny.hpp"

#include "BoardManager.hpp"
#include <common/Logger.hpp>
#include <platform/BoardListProtocol.hpp>
#include <platform/templates/searchBoardFunction.hpp>


std::unique_ptr<BoardDescriptor> BoardAny::searchBoard(uint16_t vid, uint16_t pid, bool serial, bool ethernet, bool usb, bool wiggler)
{
    BoardManager boardMgr(serial, ethernet, usb, wiggler);

    LOG(DEBUG) << "Looking for board with VID/PID: 0x" << std::hex << vid << " / 0x" << pid << " ...";
    auto it = findBoardData(vid, pid, BoardListProtocol::begin, BoardListProtocol::end);

    if (it != BoardListProtocol::end)
    {
        boardMgr.enumerate(it, it + 1, 1);
    }
    else
    {
        const BoardData d = {vid, pid, BoardFactoryFunction<BoardRemote>};
        boardMgr.enumerate(&d, &d + 1, 1);
    }

    auto &enumeratedList = boardMgr.getEnumeratedList();
    if (!enumeratedList.empty())
    {
        auto &d = enumeratedList.front();
        if ((vid == d->getVid()) && (pid == d->getPid()))
        {
            return std::move(d);
        }
    }
    throw EConnection("Board not found!");
}

std::unique_ptr<BoardInstance> BoardAny::createBoardInstance(uint16_t vid, uint16_t pid, bool serial, bool ethernet, bool usb, bool wiggler)
{
    return searchBoard(vid, pid, serial, ethernet, usb, wiggler)->createBoardInstance();
}
