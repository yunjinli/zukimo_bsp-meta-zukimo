/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <mutex>

#include <platform/bridge/BridgeProtocol.hpp>
#include <platform/usb/VendorExtensionCypressImpl.hpp>

/**
 * This class wraps the vendor implementation for Cypress.
 * It just instantiates VendorExtensionCypressImpl during construction.
  */
class VendorExtensionCypress :
    public BridgeProtocol
{
public:
    VendorExtensionCypress(IUvcExtension *uvcExtension);

private:
    VendorExtensionCypressImpl m_impl;
};
