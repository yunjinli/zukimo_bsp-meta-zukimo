/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "VendorExtensionCypress.hpp"

VendorExtensionCypress::VendorExtensionCypress(IUvcExtension *uvcExtension) :
    BridgeProtocol(&m_impl),
    m_impl(uvcExtension)
{
}
