/****************************************************************************\
* Copyright (C) 2017 Infineon Technologies
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
\****************************************************************************/
#ifndef LINK_DEFINITIONS_H
#define LINK_DEFINITIONS_H 1


// if only IPv4 is enabled on the adapter, usually Ethernet MTU of 1500 is used, but with IPv6 enabled, MTU is 1280
#define ETH_MTU                 1280    // Worst-case max. MTU to avoid fragmentation
#define ETH_IPV4_HEADER_SIZE    20      // IP header size
#define ETH_UDP_HEADER_SIZE     8       // UDP header size
#define ETH_UDP_MAX_PAYLOAD     (ETH_MTU - ETH_IPV4_HEADER_SIZE - ETH_UDP_HEADER_SIZE) // unfragmented UDP packet
//#define ETH_UDP_MAX_PAYLOAD     (65535 - ETH_IPV4_HEADER_SIZE - ETH_UDP_HEADER_SIZE) // max. UDP packet size

// For IPv4 hosts are required to support at least a maximum segment size of 536 bytes,
// see also https://en.wikipedia.org/wiki/Maximum_segment_size
// With 536 bytes the performance is _very_ bad though.
// As a result we increased the payload to 60k.
#define ETH_TCP_MAX_PAYLOAD     (1024 * 60)

// do not define a multiple of the USB CDC endpoint's wMaxPacketSize (512 for USB 2 and 1024 for USB 3)
// to avoid requiring an additional zero-length packet to finish each packet transfer in this case
#define SERIAL_MAX_PACKET_SIZE  (1024 * 4 - 1)


#define HSSL_BUFFER_SIZE        (1024 * 2)


#endif /* LINK_DEFINITIONS_H */
