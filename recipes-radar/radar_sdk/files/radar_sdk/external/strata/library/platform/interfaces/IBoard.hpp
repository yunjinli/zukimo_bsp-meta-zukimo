/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

//#include <platform/interfaces/IBridgeData.hpp>
//#include <platform/interfaces/IBridgeControl.hpp>

#include <Definitions.hpp>
#include <components/interfaces/IComponent.hpp>
#include <modules/interfaces/IModule.hpp>
#include <universal/implementations.h>

#include <cstdint>


class IBoard
{
public:
    STRATA_API virtual ~IBoard() = default;

    virtual IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl = MODULE_IMPL_DEFAULT)          = 0;
    virtual IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl = COMPONENT_IMPL_DEFAULT) = 0;

    /**
     * @brief getIComponentImplementation returns the implementation of the component specified by the parameters
     * @param type
     * @param id
     * @return the defined component implementation, or INVALID_IMPL if the component specified by the parameters does not exist
     */
    virtual uint8_t getIComponentImplementation(uint8_t type, uint8_t id) = 0;

    /**
     * @brief getIModuleImplementation returns the implementation of the module specified by the parameters
     * @param type
     * @param id
     * @return the defined module implementation, or INVALID_IMPL if the module specified by the parameters does not exist
     */
    virtual uint8_t getIModuleImplementation(uint8_t type, uint8_t id) = 0;


    template <class ComponentType>
    ComponentType *getComponent(uint8_t id)
    {
        return dynamic_cast<ComponentType *>(getIComponent(ComponentType::getType(), id, ComponentType::getImplementation()));
    }

    template <class ModuleType>
    ModuleType *getModule(uint8_t id = 0)
    {
        return dynamic_cast<ModuleType *>(getIModule(ModuleType::getType(), id, ModuleType::getImplementation()));
    }

    template <class ComponentType>
    uint8_t getComponentImplementation(uint8_t id)
    {
        return getIComponentImplementation(ComponentType::getType(), id);
    }

    template <class ModuleType>
    uint8_t getModuleImplementation(uint8_t id)
    {
        return getIModuleImplementation(ModuleType::getType(), id);
    }
};
