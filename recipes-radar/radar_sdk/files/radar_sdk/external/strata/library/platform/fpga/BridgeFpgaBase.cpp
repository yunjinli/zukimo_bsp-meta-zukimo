#include "BridgeFpgaBase.hpp"

BridgeFpgaBase::BridgeFpgaBase(const std::shared_ptr<IBridge> &bridge) :
    m_bridge(bridge)
{
}

bool BridgeFpgaBase::isConnected()
{
    return m_bridge->isConnected();
}

void BridgeFpgaBase::openConnection()
{
    m_bridge->openConnection();
}

void BridgeFpgaBase::closeConnection()
{
    m_bridge->closeConnection();
}

IBridgeControl *BridgeFpgaBase::getIBridgeControl()
{
    return m_bridge->getIBridgeControl();
}

IBridgeData *BridgeFpgaBase::getIBridgeData()
{
    return m_bridge->getIBridgeData();
}
