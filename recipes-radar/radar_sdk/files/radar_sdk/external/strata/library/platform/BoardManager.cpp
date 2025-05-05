/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BoardManager.hpp"

#include <common/Logger.hpp>
#include <common/exception/ENotImplemented.hpp>
#include <platform/BoardListProtocol.hpp>
#include <platform/exception/EAlreadyOpened.hpp>
#include <platform/exception/EConnection.hpp>
#include <platform/exception/EInUse.hpp>

#include <platform/ethernet/EnumeratorEthernetUdp.hpp>
#include <serial/EnumeratorSerialImpl.hpp>
#include <usb/EnumeratorUsbImpl.hpp>

#ifdef STRATA_USE_MCD
    #include <platform/wiggler/EnumeratorWiggler.hpp>
#endif

#include <common/cpp11/memory.hpp>
#include <cstring>


BoardManager::BoardManager(bool serial, bool ethernet, bool usb, bool wiggler) :
    m_selector {nullptr}
{
    if (serial)
    {
        m_enumerators.push_back(std::make_unique<EnumeratorSerialImpl>());
    }
    if (ethernet)
    {
        m_enumerators.push_back(std::make_unique<EnumeratorEthernetUdp>());
    }
    if (usb)
    {
        m_enumerators.push_back(std::make_unique<EnumeratorUsbImpl>());
    }
    if (wiggler)
    {
#ifdef STRATA_USE_MCD
        m_enumerators.push_back(std::make_unique<EnumeratorWiggler>());
#else
        throw ENotImplemented();
#endif
    }
}

BoardManager::~BoardManager()
{
}

void BoardManager::setEnumerationSelector(IEnumerationSelector *selector)
{
    m_selector = selector;
}

uint16_t BoardManager::enumerate(uint16_t maxCount)
{
    return enumerate(BoardListProtocol::begin, BoardListProtocol::end, maxCount);
}

uint16_t BoardManager::enumerate(BoardData::const_iterator begin, BoardData::const_iterator end, uint16_t maxCount)
{
    m_maxCount = maxCount;
    m_enumeratedList.clear();

    for (auto &e : m_enumerators)
    {
        e->enumerate(*this, begin, end);
        if (m_maxCount && (m_enumeratedList.size() >= m_maxCount))
        {
            break;
        }
    }

    return static_cast<uint16_t>(m_enumeratedList.size());
}

BoardDescriptorList &BoardManager::getEnumeratedList()
{
    return m_enumeratedList;
}

std::unique_ptr<BoardInstance> BoardManager::createBoardInstance()
{
    if (m_enumeratedList.empty())
    {
        throw EConnection("No boards enumerated");
    }

    for (auto &d : m_enumeratedList)
    {
        LOG(DEBUG) << "Opening unused board ...";
        try
        {
            return d->createBoardInstance();
        }
        catch (const EAlreadyOpened &)
        {
            LOG(DEBUG) << "... board already used";
            continue;
        }
    }

    throw EConnection("... no unused board found");
}

std::unique_ptr<BoardInstance> BoardManager::createBoardInstance(uint8_t index)
{
    if (m_enumeratedList.empty())
    {
        throw EConnection("No boards enumerated");
    }
    if (index >= m_enumeratedList.size())
    {
        throw EConnection("Specified board index not found");
    }

    try
    {
        const auto &d = m_enumeratedList[index];
        return d->createBoardInstance();
    }
    catch (const EAlreadyOpened &)
    {
        throw EConnection("Specified board already in use");
    }
}

std::unique_ptr<BoardInstance> BoardManager::createBoardInstance(uint16_t vid, uint16_t pid)
{
    if (m_enumeratedList.empty())
    {
        throw EConnection("No boards enumerated");
    }

    for (auto &d : m_enumeratedList)
    {
        if ((d->getVid() == vid) && (d->getPid() == pid))
        {
            try
            {
                return d->createBoardInstance();
            }
            catch (const EAlreadyOpened &)
            {
                throw EConnection("Specified board already in use");
            }
        }
    }

    throw EConnection("Specified board not found");
}

std::unique_ptr<BoardInstance> BoardManager::createBoardInstance(const char name[])
{
    if (m_enumeratedList.empty())
    {
        throw EConnection("No boards enumerated");
    }

    for (auto &d : m_enumeratedList)
    {
        if (!strcmp(d->getName(), name))
        {
            try
            {
                return d->createBoardInstance();
            }
            catch (const EInUse &)
            {
                throw EConnection("Specified board already in use");
            }
        }
    }

    throw EConnection("Specified board not found");
}

STRATA_API std::unique_ptr<BoardInstance> BoardManager::createSpecificBoardInstance(const uint8_t uuid[UUID_LENGTH])
{
    if (m_enumeratedList.empty())
    {
        throw EConnection("No boards enumerated");
    }

    for (auto &d : m_enumeratedList)
    {
        LOG(DEBUG) << "Opening unused board ...";
        try
        {
            const auto boardUuid = d->getUuid();
            if (std::equal(boardUuid.begin(), boardUuid.end(), uuid))
            {
                return d->createBoardInstance();
            }
        }
        catch (const EAlreadyOpened &)
        {
            LOG(DEBUG) << "... board already used";
            continue;
        }
    }

    throw EConnection("Specified board not found");
}

bool BoardManager::onEnumerate(std::unique_ptr<BoardDescriptor> &&descriptor)
{
    bool select;
    if (m_selector)
    {
        select = m_selector->select(descriptor.get());
    }
    else
    {
        select = true;
    }

    if (select)
    {
        m_enumeratedList.push_back(std::move(descriptor));
    }

    return (m_maxCount && (m_enumeratedList.size() >= m_maxCount));
}
