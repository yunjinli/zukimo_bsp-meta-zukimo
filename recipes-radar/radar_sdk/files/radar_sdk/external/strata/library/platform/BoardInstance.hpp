/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <Definitions.hpp>
#include <platform/interfaces/IBoard.hpp>
#include <platform/interfaces/IBridge.hpp>
#include <platform/interfaces/IVendorCommands.hpp>

#include <memory>


class BoardInstance :
    public IBoard
{
public:
    BoardInstance(std::shared_ptr<IBridge> bridge, std::unique_ptr<IBoard> board, const std::string &name, uint16_t vid = 0, uint16_t pid = 0);

    inline std::shared_ptr<IBridge> getIBridge()
    {
        return m_bridge;
    }

    inline const uint16_t &getVid()
    {
        return m_vid;
    }

    inline const uint16_t &getPid()
    {
        return m_pid;
    }

    inline const char *getName()
    {
        return m_name.c_str();
    }

    inline const std::string &getVersionString()
    {
        return m_bridge->getIBridgeControl()->getVersionString();
    }

    inline const std::string &getExtendedVersionString()
    {
        return m_bridge->getIBridgeControl()->getExtendedVersionString();
    }

    inline const IBridgeControl::VersionInfo_t &getVersionInfo()
    {
        return m_bridge->getIBridgeControl()->getVersionInfo();
    }

    inline const IBridgeControl::Uuid_t &getUuid()
    {
        return m_bridge->getIBridgeControl()->getUuid();
    }

    inline IVendorCommands *getIVendorCommands()
    {
        return m_bridge->getIBridgeControl()->getIVendorCommands();
    }

    inline IGpio *getIGpio()
    {
        return m_bridge->getIBridgeControl()->getIGpio();
    }

    inline II2c *getII2c()
    {
        return m_bridge->getIBridgeControl()->getII2c();
    }

    inline ISpi *getISpi()
    {
        return m_bridge->getIBridgeControl()->getISpi();
    }

    inline IFlash *getIFlash()
    {
        return m_bridge->getIBridgeControl()->getIFlash();
    }

    inline IMemory<uint32_t> *getIMemory()
    {
        return m_bridge->getIBridgeControl()->getIMemory();
    }

    inline IData *getIData()
    {
        return m_bridge->getIBridgeControl()->getIData();
    }

    inline void registerListener(IFrameListener<> *listener)
    {
        m_bridge->getIBridgeData()->registerListener(listener);
    }

    inline IFrame *getFrame(uint16_t timeoutMs = 5000)
    {
        return m_bridge->getIBridgeData()->getFrame(timeoutMs);
    }

    /**
     * @brief Obtain the specific interface of a bridge to allow access to its implementation.
     * @note The bridge must support this by deriving from IBridgeSpecificInterface.
     * @return The specific interface of the bridge or null if there is none.
     */
    template <typename InterfaceType>
    inline InterfaceType *getBridgeSpecificInterface()
    {
        return m_bridge->getSpecificInterface<InterfaceType>();
    }

    // IBoard
    STRATA_API IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl = MODULE_IMPL_DEFAULT) override;
    STRATA_API IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl = COMPONENT_IMPL_DEFAULT) override;

    STRATA_API uint8_t getIComponentImplementation(uint8_t type, uint8_t id) override;
    STRATA_API uint8_t getIModuleImplementation(uint8_t type, uint8_t id) override;

private:
    // this has to be the first member, so it is destructed last (pointer may be used by IBoard instance until its destruction)
    std::shared_ptr<IBridge> m_bridge;

    const uint16_t m_vid, m_pid;
    const std::string m_name;

    std::unique_ptr<IBoard> m_board;
};
