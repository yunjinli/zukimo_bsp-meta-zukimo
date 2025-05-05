/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRegisters.hpp"
#include <algorithm>
#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/iregisters.h>
#include <universal/protocol/protocol_definitions.h>
#include <vector>

template <typename AddressType, typename ValueType>
RemoteRegisters<AddressType, ValueType>::RemoteRegisters(AddressType increment, IVendorCommands *commands, uint8_t bType, uint8_t bImplementation, uint8_t id) :
    Registers<AddressType, ValueType>(increment),
    m_vendorCommands {commands, CMD_COMPONENT, bType, bImplementation, id, COMPONENT_SUBIF_REGISTERS}
{
}

template <typename AddressType, typename ValueType>
ValueType RemoteRegisters<AddressType, ValueType>::read(AddressType address)
{
    ValueType value;
    read(address, 1, &value);
    return value;
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::read(AddressType address, ValueType &value)
{
    read(address, 1, &value);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::write(AddressType address, ValueType value)
{
    write(address, 1, &value);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::read(AddressType address, AddressType count, ValueType values[])
{
    const auto increment = Registers<AddressType, ValueType>::m_increment;

    constexpr uint16_t elemSize = sizeof(values[0]);
    constexpr uint16_t argSize  = sizeof(address) + sizeof(count);
    uint8_t payload[argSize];
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t wLength       = wCount * elemSize;
        uint8_t *it                  = payload;
        it                           = hostToSerial(it, address);
        it                           = hostToSerial(it, wCount);

        m_vendorCommands.vendorTransferChecked(FN_REGISTERS_READ_BURST, argSize, payload, wLength, reinterpret_cast<uint8_t *>(values));

        address += (wCount * increment);
        values += wCount;
        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::write(AddressType address, AddressType count, const ValueType values[])
{
    const auto increment = Registers<AddressType, ValueType>::m_increment;

    constexpr uint16_t elemSize    = sizeof(values[0]);
    constexpr uint16_t argSize     = sizeof(address);
    const decltype(count) maxCount = (m_vendorCommands.getMaxTransfer() - argSize) / elemSize;
    stdext::buffer<uint8_t> payload(std::min(maxCount * elemSize, count * elemSize) + argSize);

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t length        = wCount * elemSize;
        const uint16_t wLength       = length + argSize;
        uint8_t *it                  = payload.data();
        it                           = hostToSerial(it, values, values + wCount);
        it                           = hostToSerial(it, address);

        m_vendorCommands.vendorWrite(FN_REGISTERS_WRITE_BURST, wLength, payload.data());

        address += (wCount * increment);
        values += wCount;
        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::readBatch(const AddressType addresses[], AddressType count, ValueType values[])
{
    constexpr uint16_t elemSize    = sizeof(ValueType);
    constexpr uint16_t argSize     = sizeof(AddressType);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const auto maxLength           = std::min(maxCount, count) * elemSize;
    stdext::buffer<uint8_t> payload(maxLength);

    while (count > 0)
    {
        const auto wCount             = std::min(count, maxCount);
        const uint16_t wLengthSend    = wCount * argSize;
        const uint16_t wLengthReceive = wCount * elemSize;
        uint8_t *it                   = payload.data();
        it                            = hostToSerial(it, addresses, addresses + wCount);

        m_vendorCommands.vendorTransferChecked(FN_REGISTERS_BATCH, wLengthSend, payload.data(), wLengthReceive, reinterpret_cast<uint8_t *>(values));

        addresses += wCount;
        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::writeBatchImpl(const BatchType vals[], AddressType count)
{
    constexpr uint16_t elemSize    = sizeof(AddressType) + sizeof(ValueType);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const auto maxLength           = std::min(maxCount, count) * elemSize;
    stdext::buffer<uint8_t> payload(maxLength);

    while (count > 0)
    {
        const auto wCount      = std::min(count, maxCount);
        const uint16_t wLength = wCount * elemSize;
        uint8_t *it            = payload.data();

        const auto last = vals + wCount;
        while (vals < last)
        {
            it = hostToSerial(it, vals->address);
            it = hostToSerial(it, vals->value);
            vals++;
        }

        m_vendorCommands.vendorWrite(FN_REGISTERS_BATCH, wLength, payload.data());

        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::setBits(AddressType address, ValueType bitmask)
{
    const uint16_t wLength = sizeof(address) + sizeof(bitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, address);
    it          = hostToSerial(it, bitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_SET_BITS, wLength, payload);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::clearBits(AddressType address, ValueType bitmask)
{
    const uint16_t wLength = sizeof(address) + sizeof(bitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, address);
    it          = hostToSerial(it, bitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_CLEAR_BITS, wLength, payload);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::modifyBits(AddressType address, ValueType clearBitmask, ValueType setBitmask)
{
    const uint16_t wLength = sizeof(address) + sizeof(setBitmask) + sizeof(clearBitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, address);
    it          = hostToSerial(it, clearBitmask);
    it          = hostToSerial(it, setBitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_MODIFY_BITS, wLength, payload);
}


/**
 * This avoids having to have the complete implementation in the header file,
 * since the linker of an external executable needs to find the implementation
 * of the class. It has to be after the member declaration, since some compilers
 * otherwise would not emit the symbols.
 */
template class RemoteRegisters<uint8_t>;
template class RemoteRegisters<uint8_t, uint16_t>;
template class RemoteRegisters<uint8_t, uint32_t>;
template class RemoteRegisters<uint16_t>;
