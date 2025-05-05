/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/nonvolatileMemory/NonvolatileMemoryFlash.hpp>
#include <platform/interfaces/access/IFlash.hpp>
#include <platform/interfaces/access/ISpi.hpp>


class FlashSpi :
    public IFlash
{
public:
    FlashSpi(ISpi *access);
    ~FlashSpi();

    uint32_t getMaxTransfer() const override;
    void read(uint8_t devId, uint32_t address, uint32_t length, uint8_t buffer[]) override;
    void write(uint8_t devId, uint32_t address, uint32_t length, const uint8_t buffer[]) override;
    void erase(uint8_t devId, uint32_t address) override;
    uint8_t getStatus(uint8_t devId) override;

private:
    inline void readCommand(uint8_t devId, uint8_t command, uint32_t *address, uint32_t length, uint8_t buffer[]);
    inline void writeCommand(uint8_t devId, uint8_t command, uint32_t *address = nullptr, uint32_t length = 0, const uint8_t buffer[] = nullptr);

    ISpi *m_access;
};


class NonvolatileMemoryFlashSpi :
    private FlashSpi,
    public NonvolatileMemoryFlash
{
public:
    NonvolatileMemoryFlashSpi(ISpi *access, uint8_t devId, uint32_t speed, const NonvolatileMemoryConfig_t &config);
    virtual ~NonvolatileMemoryFlashSpi();
};
