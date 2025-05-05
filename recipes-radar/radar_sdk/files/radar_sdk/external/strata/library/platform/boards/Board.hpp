/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/interfaces/IBoard.hpp>

#include <map>
#include <utility>


class Board :
    public IBoard
{
public:
    IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl) override;
    IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl) override;

    uint8_t getIComponentImplementation(uint8_t type, uint8_t id) override;
    uint8_t getIModuleImplementation(uint8_t type, uint8_t id) override;


protected:
    using KeyType = uint16_t;

    template <typename I>
    using InstanceEntry = std::pair<uint8_t, I *>;

    template <typename T>
    inline void registerInstance(T &instance, uint8_t id)
    {
        auto &map         = get_map(&instance);
        const auto type   = T::getType();
        const KeyType key = (type << 8) | id;
        map[key]          = {T::getImplementation(), &instance};
    }

    template <typename I>
    inline InstanceEntry<I> *getInstanceEntry(uint8_t type, uint8_t id)
    {
        auto &map         = get_map(static_cast<I *>(0));
        const KeyType key = (type << 8) | id;
        auto entry        = map.find(key);
        if (entry != map.end())
        {
            return &entry->second;
        }
        return nullptr;
    }

private:
    template <typename I>
    inline I *getInstance(uint8_t type, uint8_t id, uint8_t impl);

    template <typename I>
    inline uint8_t getInstanceHelper(uint8_t type, uint8_t id);

    std::map<KeyType, InstanceEntry<IComponent>> m_components;
    std::map<KeyType, InstanceEntry<IModule>> m_modules;

    inline decltype(m_components) &get_map(IComponent *)
    {
        return m_components;
    }
    inline decltype(m_modules) &get_map(IModule *)
    {
        return m_modules;
    }
};
