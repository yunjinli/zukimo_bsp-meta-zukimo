/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BoardInstance.hpp"


BoardInstance::BoardInstance(std::shared_ptr<IBridge> bridge, std::unique_ptr<IBoard> board, const std::string &name, uint16_t vid, uint16_t pid) :
    m_bridge {bridge},
    m_vid {vid},
    m_pid {pid},
    m_name {name},
    m_board {std::move(board)}
{
}

IModule *BoardInstance::getIModule(uint8_t type, uint8_t id, uint8_t impl)
{
    return m_board->getIModule(type, id, impl);
}

IComponent *BoardInstance::getIComponent(uint8_t type, uint8_t id, uint8_t impl)
{
    return m_board->getIComponent(type, id, impl);
}

uint8_t BoardInstance::getIComponentImplementation(uint8_t type, uint8_t id)
{
    return m_board->getIComponentImplementation(type, id);
}

uint8_t BoardInstance::getIModuleImplementation(uint8_t type, uint8_t id)
{
    return m_board->getIModuleImplementation(type, id);
}
