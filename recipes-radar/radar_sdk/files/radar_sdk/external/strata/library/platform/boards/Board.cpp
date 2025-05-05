/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Board.hpp"


template <typename I>
inline I *Board::getInstance(uint8_t type, uint8_t id, uint8_t impl)
{
    auto entry = getInstanceEntry<I>(type, id);
    if (entry)
    {
        if ((impl == DEFAULT_IMPL) || (impl == entry->first))
        {
            return entry->second;
        }
    }
    return nullptr;
}

template <typename I>
inline uint8_t Board::getInstanceHelper(uint8_t type, uint8_t id)
{
    auto entry = getInstanceEntry<I>(type, id);
    if (entry)
    {
        return entry->first;
    }
    return INVALID_IMPL;
}

IComponent *Board::getIComponent(uint8_t type, uint8_t id, uint8_t impl)
{
    return getInstance<IComponent>(type, id, impl);
}

IModule *Board::getIModule(uint8_t type, uint8_t id, uint8_t impl)
{
    return getInstance<IModule>(type, id, impl);
}

uint8_t Board::getIComponentImplementation(uint8_t type, uint8_t id)
{
    return getInstanceHelper<IComponent>(type, id);
}

uint8_t Board::getIModuleImplementation(uint8_t type, uint8_t id)
{
    return getInstanceHelper<IModule>(type, id);
}
