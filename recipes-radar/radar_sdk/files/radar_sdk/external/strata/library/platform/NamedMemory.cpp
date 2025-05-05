/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "NamedMemory.hpp"
#include <fstream>
#include <iomanip>
#include <limits>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

template <typename AddressType, typename ValueType>
NamedMemory<AddressType, ValueType>::NamedMemory(IMemory<AddressType, ValueType> *memory, const char registerFile[]) :
    m_memory {memory}
{
    loadFile(registerFile);
}

template <typename AddressType, typename ValueType>
NamedMemory<AddressType, ValueType>::NamedMemory(IMemory<AddressType, ValueType> *memory, const std::string &registerFile) :
    m_memory {memory}
{
    loadFile(registerFile.c_str());
}

template <typename AddressType, typename ValueType>
NamedMemory<AddressType, ValueType>::NamedMemory(IMemory<AddressType, ValueType> *memory, const Addresses &nameMap) :
    m_memory {memory},
    m_names {nameMap}
{}

template <typename AddressType, typename ValueType>
NamedMemory<AddressType, ValueType>::NamedMemory(IMemory<AddressType, ValueType> *memory, Addresses &&nameMap) :
    m_memory {memory},
    m_names {std::move(nameMap)}
{}

template <typename AddressType, typename ValueType>
IMemory<AddressType, ValueType> *NamedMemory<AddressType, ValueType>::getIMemory() const
{
    return m_memory;
}

template <typename AddressType, typename ValueType>
const typename NamedMemory<AddressType, ValueType>::Bitfield *NamedMemory<AddressType, ValueType>::getAddress(const std::string &name, AddressType &address)
{
    if (name.front() == '0')
    {
        // we have a number as string, so try to convert it
        address = std::stoi(name);
        return nullptr;
    }

    const auto separator = name.find('.');
    if (separator != std::string::npos)
    {
        // we have a bitfield access
        const auto parentName   = name.substr(0, separator);
        const auto bitfieldName = name.substr(separator + 1);
        const auto &entry       = m_names.at(parentName);

        address = entry.address;
        return &entry.bitfields.at(bitfieldName);
    }

    // we just have a simple name to look up
    address = m_names.at(name).address;
    return nullptr;
}

template <typename AddressType, typename ValueType>
const std::string &NamedMemory<AddressType, ValueType>::getName(AddressType address)
{
    static const std::string notFound = "NOT_FOUND";

    auto it = std::find_if(m_names.begin(), m_names.end(), [address](const std::pair<std::string, Address> &p) {
        return p.second.address == address;
    });
    if (it == m_names.end())
    {
        return notFound;
    }
    else
    {
        return it->first;
    }
}

template <typename AddressType, typename ValueType>
void NamedMemory<AddressType, ValueType>::loadFile(const std::string &filename)
{
    if (filename.substr(filename.size() - 4) == ".xml")
    {
    }
    else if (filename.substr(filename.size() - 5) == ".json")
    {
        loadJson(filename);
    }
    else
    {
        throw "unknown file extension";
    }
}

template <typename AddressType, typename ValueType>
void NamedMemory<AddressType, ValueType>::loadJson(const std::string &filename)
{
    rapidjson::Document document;
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        throw EMemory("NamedMemory<>::loadJson() - file open failed");
    }

    rapidjson::IStreamWrapper isw(ifs);
    document.ParseStream(isw);

    if (document.HasParseError())
    {
        throw EMemory("JSON parsing failed");
    }

    if (!document.HasMember("units") && !document.HasMember("registers"))
    {
        throw EMemory("JSON member units or registers missing");
    }

    auto registers = document["units"][0]["registers"].GetArray();

    for (const auto &reg : registers)
    {
        Bitfields bitfieldList;

        const auto register_name = reg["name"].GetString();
        const auto address       = reg["address"].GetInt();
        const auto reset_val     = reg["reset"][0]["value"].GetInt();

        const auto bslices = reg["bslices"].GetArray();
        for (const auto &bslice : bslices)
        {
            const auto bslice_name = bslice["name"].GetString();
            const auto width       = bslice["width"].GetInt();
            const auto offset      = bslice["offset"].GetInt();
            const auto mask        = ((1 << width) - 1) << offset;
            const auto reserved    = strstr("RSVD", bslice_name);
            if (!reserved)
            {
                bitfieldList.insert({bslice_name, {static_cast<AddressType>(mask), static_cast<AddressType>(offset)}});
            }
        }
        m_names.insert({register_name, {static_cast<AddressType>(address), static_cast<ValueType>(reset_val), bitfieldList}});
    }
}

template <typename AddressType, typename ValueType>
std::vector<typename NamedMemory<AddressType, ValueType>::BatchType> NamedMemory<AddressType, ValueType>::loadConfig(const std::string &filename)
{
    std::ifstream ifs(filename);
    std::vector<BatchType> registerList;

    if (!ifs.is_open())
    {
        throw EMemory("NamedMemory<>::loadConfig() - file open failed");
    }
    while (ifs.good())
    {
        BatchType registerEntry;
        ifs.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        ifs >> std::hex >> registerEntry.address >> std::hex >> registerEntry.value;
        registerList.emplace_back(registerEntry);
    }
    ifs.close();
    return registerList;
}

template <typename AddressType, typename ValueType>
void NamedMemory<AddressType, ValueType>::saveConfig(const std::string &filename, const std::vector<typename NamedMemory<AddressType, ValueType>::BatchType> &registerList)
{
    std::ofstream outfile;
    outfile.open(filename, std::ofstream::out);
    if (!outfile.is_open())
    {
        throw EMemory("NamedMemory<>::saveConfig() - file open failed");
    }

    for (auto &entry : registerList)
    {
        const auto &name = getName(entry.address);
        toString(outfile, name, entry);
    }
    outfile.close();
}

template <typename AddressType, typename ValueType>
void NamedMemory<AddressType, ValueType>::toString(std::ostream &stream, const std::string &registerName, const BatchType &registerPair)
{
    stream << std::hex << std::setfill('0');
    stream << registerName << " " << std::setw(4) << registerPair.address << " " << std::setw(4) << registerPair.value << '\n';
}


/**
 * This avoids having to have the complete implementation in the header file,
 * since the linker of an external executable needs to find the implementation
 * of the class. It has to be after the member declaration, since some compilers
 * otherwise would not emit the symbols.
 */
template class NamedMemory<uint8_t>;
template class NamedMemory<uint8_t, uint16_t>;
template class NamedMemory<uint8_t, uint32_t>;
template class NamedMemory<uint16_t>;
template class NamedMemory<uint32_t>;
template class NamedMemory<uint32_t, uint8_t>;
