/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/exception/EMemory.hpp>
#include <platform/interfaces/access/IMemory.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <vector>


template <typename AddressType, typename ValueType = AddressType>
class NamedMemory
{
public:
    using BatchType      = typename IMemory<AddressType, ValueType>::BatchType;
    using NamedBatchType = std::pair<std::string, ValueType>;

    struct Bitfield
    {
        AddressType mask;
        AddressType offset;
        //        std::string desc;
        //        std::string info;
    };
    using Bitfields = std::map<std::string, Bitfield>;

    struct Address
    {
        AddressType address;
        ValueType reset_value;
        Bitfields bitfields;
    };
    //    using Address   = std::pair<AddressType, Bitfields>;
    using Addresses = std::map<std::string, Address>;


    NamedMemory(IMemory<AddressType, ValueType> *memory, const char registerFile[]);
    NamedMemory(IMemory<AddressType, ValueType> *memory, const std::string &registerFile);
    NamedMemory(IMemory<AddressType, ValueType> *memory, const Addresses &nameMap);
    NamedMemory(IMemory<AddressType, ValueType> *memory, Addresses &&nameMap);


    ValueType read(AddressType address)
    {
        return m_memory->read(address);
    }

    ValueType read(const std::string &name)
    {
        ValueType value;
        read(name, value);
        return value;
    }

    void read(const std::string &name, ValueType &value)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);
        value               = m_memory->read(address);
        if (bitfield)
        {
            value &= bitfield->mask;
            value >>= bitfield->offset;
        }
    }

    void read(std::vector<std::string> names, ValueType values[])
    {
        for (const auto &name : names)
        {
            AddressType address;
            const auto bitfield = getAddress(name, address);
            *values             = read(address);
            if (bitfield)
            {
                *values &= bitfield->mask;
                *values >>= bitfield->offset;
            }
            values++;
        }
    }

    template <typename... Args>
    void read(const std::string &name, Args &&...args)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);
        if (bitfield)
        {
            throw EMemory("Bitfield access cannot be used for burst read");
        }

        m_memory->read(address, std::forward<Args>(args)...);
    }


    void write(const std::string &name, ValueType value)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);
        if (bitfield)
        {
            const ValueType clearMask = bitfield->mask;
            const ValueType setMask   = value << bitfield->offset;
            m_memory->modifyBits(address, clearMask, setMask);
        }
        else
        {
            m_memory->write(address, value);
        }
    }

    void write(const std::vector<NamedBatchType> &values, bool optimize = false)
    {
        std::vector<BatchType> chunk;
        for (auto &entry : values)
        {
            AddressType address;
            const auto bitfield = getAddress(entry.first, address);
            if (bitfield)
            {
                if (!chunk.empty())
                {
                    m_memory->writeBatch(chunk.data(), static_cast<AddressType>(chunk.size()), optimize);
                    chunk.clear();
                }
                const ValueType setMask = entry.second << bitfield->offset;
                m_memory->modifyBits(address, bitfield->mask, setMask);
            }
            else
            {
                chunk.emplace_back(BatchType {address, entry.second});
            }
        }
        if (!chunk.empty())
        {
            m_memory->writeBatch(chunk.data(), static_cast<AddressType>(chunk.size()), optimize);
        }
    }

    void setBits(const std::string &name, ValueType &bitmask)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);

        if (bitfield)
        {
            bitmask <<= bitfield->offset;
            bitmask &= bitfield->mask;
        }
        m_memory->setBits(address, bitmask);
    }

    void clearBits(const std::string &name, ValueType &bitmask)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);

        if (bitfield)
        {
            bitmask <<= bitfield->offset;
            bitmask &= bitfield->mask;
        }
        m_memory->clearBits(address, bitmask);
    }

    void modifyBits(const std::string &name, ValueType clearBitmask, ValueType setBitmask)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);

        if (bitfield)
        {
            clearBitmask <<= bitfield->offset;
            clearBitmask &= bitfield->mask;
            setBitmask <<= bitfield->offset;
            setBitmask &= bitfield->mask;
        }
        m_memory->modifyBits(address, clearBitmask, setBitmask);
    }

    template <typename... Args>
    void write(const std::string &name, Args &&...args)
    {
        AddressType address;
        const auto bitfield = getAddress(name, address);
        if (bitfield)
        {
            throw EMemory("Bitfield access cannot be used for burst write");
        }

        m_memory->write(address, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    void read(T address, Args &&...args)
    {
        m_memory->read(address, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    void write(T address, Args &&...args)
    {
        m_memory->write(address, std::forward<Args>(args)...);
    }

    std::vector<typename IMemory<AddressType, ValueType>::BatchType> loadConfig(const std::string &filename);
    void saveConfig(const std::string &filename, const std::vector<typename IMemory<AddressType, ValueType>::BatchType> &registerList);

    IMemory<AddressType, ValueType> *getIMemory() const;

private:
    ///
    /// \brief getAddress
    /// \param name string containing the name for the address (optionally with a '.' separated bitfield name) or an address
    /// \param address the numeric value for the address
    /// \return pointer to the bitfield (optional), or nullptr if no bitfield was specified
    ///
    const Bitfield *getAddress(const std::string &name, AddressType &address);

    const std::string &getName(AddressType address);

    void toString(std::ostream &stream, const std::string &registerName, const BatchType &registerPair);

    void loadFile(const std::string &filename);
    void loadJson(const std::string &filename);

    IMemory<AddressType, ValueType> *m_memory;
    std::map<std::string, Address> m_names;
};


/**
 * This is for the linker of an external executable to find the implementation
 * of the class. Otherwise the complete implementation would have to be in the header file.
 */
extern template class NamedMemory<uint8_t>;
extern template class NamedMemory<uint8_t, uint16_t>;
extern template class NamedMemory<uint8_t, uint32_t>;
extern template class NamedMemory<uint16_t>;
extern template class NamedMemory<uint32_t>;
extern template class NamedMemory<uint32_t, uint8_t>;
