/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BoardRemote.hpp"

#include <common/exception/ENotImplemented.hpp>
#include <platform/exception/EConnection.hpp>
#include <platform/exception/EProtocolFunction.hpp>
#include <remote/RemoteModuleRadar.hpp>
#include <remote/RemoteProcessingRadar.hpp>
#include <remote/RemoteRadarAtr22.hpp>
#include <remote/RemoteRadarAvian.hpp>
#include <remote/RemoteRadarLtr11.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/modules/subinterfaces.h>
#include <universal/protocol/protocol_definitions.h>


BoardRemote::BoardRemote(IBridge *bridge) :
    m_bridge {bridge}
{
    m_commands = bridge->getIBridgeControl()->getIVendorCommands();
    if (m_commands == nullptr)
    {
        throw EConnection("IVendorCommands not implemented in this Bridge");
    }
}

IComponent *BoardRemote::createComponent(uint8_t type, uint8_t id, uint8_t &impl)
{
    impl = getComponentImplementation(type, id);

    return createComponentFunction(type, id, impl);
}

IModule *BoardRemote::createModule(uint8_t type, uint8_t id, uint8_t &impl)
{
#if MODULE_IMPL_INTERFACE_IMPLEMENTED_IN_FIRMWARE
    impl = getModuleImplementation(type, id);
#else
    impl = MODULE_IMPL_DEFAULT;
#endif
    return createModuleFunction(type, id, impl);
}

IComponent *BoardRemote::createComponentFunction(uint8_t type, uint8_t id, uint8_t impl)
{
    switch (type)
    {
        case COMPONENT_TYPE_RADAR:
            switch (impl)
            {
                case COMPONENT_IMPL_RADAR_AVIAN:
                    return createComponentHelper<RemoteRadarAvian>(id);
                    break;
                case COMPONENT_IMPL_RADAR_ATR22:
                    return createComponentHelper<RemoteRadarAtr22>(id);
                    break;
                case COMPONENT_IMPL_RADAR_LTR11:
                    return createComponentHelper<RemoteRadarLtr11>(id);
                    break;
                default:
                    return createComponentHelper<RemoteRadar>(id);
                    break;
            }
            break;
        case COMPONENT_TYPE_PROCESSING_RADAR:
            return createComponentHelper<RemoteProcessingRadar>(id);
            break;
        default:
            break;
    }

    return nullptr;
}

IModule *BoardRemote::createModuleFunction(uint8_t type, uint8_t id, uint8_t /*impl*/)
{
    switch (type)
    {
        case MODULE_TYPE_RADAR:
            return createModuleHelper<RemoteModuleRadar>(id);
            break;
        default:
            break;
    }

    return nullptr;
}

IComponent *BoardRemote::getIComponent(uint8_t type, uint8_t id, uint8_t impl)
{
    auto entry = getInstanceEntry<IComponent>(type, id);
    if (entry)
    {
        // check if the found implementation matches the requested one (or if the default was requested)
        if ((impl == DEFAULT_IMPL) || (impl == entry->first))
        {
            return entry->second;
        }

        return nullptr;
    }
    else
    {
        // we haven't found the component, so try to create a remote instance
        uint8_t createdImpl;
        auto instance = createComponent(type, id, createdImpl);

        // check if the created implementation matches the requested one (or if the default was requested)
        if ((impl == DEFAULT_IMPL) || (impl == createdImpl))
        {
            return instance;
        }

        return nullptr;
    }
}

IModule *BoardRemote::getIModule(uint8_t type, uint8_t id, uint8_t impl)
{
    auto entry = getInstanceEntry<IModule>(type, id);
    if (entry)
    {
        // check if the found implementation matches the requested one (or if the default was requested)
        if ((impl == DEFAULT_IMPL) || (impl == entry->first))
        {
            return entry->second;
        }

        return nullptr;
    }
    else
    {
        // we haven't found the component, so try to create a remote instance
        uint8_t createdImpl;
        auto instance = createModule(type, id, createdImpl);

        // check if the created implementation matches the requested one (or if the default was requested)
        if ((impl == 0) || (impl == createdImpl))
        {
            return instance;
        }

        return nullptr;
    }
}

uint8_t BoardRemote::getIComponentImplementation(uint8_t type, uint8_t id)
{
    auto entry = getInstanceEntry<IComponent>(type, id);
    if (entry)
    {
        return entry->first;
    }
    else
    {
        // we haven't found the component, so try to create the remote instance
        uint8_t createdImpl;
        auto component = createComponent(type, id, createdImpl);
        if (component)
        {
            // only return the implementation if a component was actually created
            return createdImpl;
        }
    }

    return INVALID_IMPL;
}


uint8_t BoardRemote::getIModuleImplementation(uint8_t /*type*/, uint8_t /*id*/)
{
#if MODULE_IMPL_INTERFACE_IMPLEMENTED_IN_FIRMWARE

#else
    throw ENotImplemented();
#endif
}

uint8_t BoardRemote::getComponentImplementation(uint8_t type, uint8_t id)
{
    try
    {
        // try to read remote implementation (and implicitly check if it exists)
        RemoteVendorCommands commands(m_commands, CMD_COMPONENT, type, COMPONENT_IMPL_DEFAULT, id, COMPONENT_SUBIF_DEFAULT);
        uint8_t impl;
        commands.vendorRead(FN_GET_IMPL, sizeof(impl), &impl);
        return impl;
    }
    catch (const EProtocolFunction &)
    {
        return INVALID_IMPL;
    }
}

uint8_t BoardRemote::getModuleImplementation(uint8_t type, uint8_t id)
{
    try
    {
        // try to read remote implementation (and implicitly check if it exists)
        RemoteVendorCommands commands(m_commands, CMD_MODULE, type, MODULE_IMPL_DEFAULT, id, MODULE_SUBIF_DEFAULT);
        uint8_t impl;
        commands.vendorRead(FN_GET_IMPL, sizeof(impl), &impl);
        return impl;
    }
    catch (const EProtocolFunction &)
    {
        return INVALID_IMPL;
    }
}
