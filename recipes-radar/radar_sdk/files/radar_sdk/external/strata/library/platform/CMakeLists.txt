
set(PLATFORM_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardAny.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardDescriptor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardInstance.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardListProtocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/NamedMemory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeControl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolI2c.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolGpio.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolSpi.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolMemory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolFlash.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/VendorImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BoardEthernetTcp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BoardEthernetUdp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernet.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetControl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetTcp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetUdp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/EnumeratorEthernetUdp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/SocketTcp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/SocketUdp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EAlreadyOpened.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EBridgeData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EConnection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EInUse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EMemory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EProtocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EProtocolFunction.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EUvcProperty.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/fpga/BridgeFpgaIrpli.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fpga/BridgeFpgaBase.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/DebugFrame.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/ErrorFrame.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/Frame.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameBase.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameForwarder.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameListenerCaller.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FramePool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameQueue.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IBoard.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IBridge.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IBridgeControl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IBridgeData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IVendorCommands.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IEnumerator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IFrame.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IFrameListener.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IFramePool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IFrameQueue.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/IData.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/IGpio.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/IMemory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/II2c.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/ISpi.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/access/IFlash.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/link/ISerialPort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/link/ISocket.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/link/IMulticoreDebugger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/link/IUvcExtension.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/BoardSerial.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/BridgeSerial.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/SerialPort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/boardFactoryFunction.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/enumerateFunction.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/identifyBoardFunction.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/searchBoardFunction.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/BoardUsb.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionList.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionCypress.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionCypressImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtek.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtekFlash.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtekI2c.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/boards/Board.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/boards/BoardGeneric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/boards/BoardRemote.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketTcpImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketUdpImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/serial/SerialPortImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/serial/EnumeratorSerialImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeUsbImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorUsbImpl.hpp"
    )

set(PLATFORM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardAny.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardDescriptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardInstance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoardManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/NamedMemory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolI2c.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolGpio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolSpi.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolMemory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolFlash.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/BridgeProtocolData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bridge/VendorImpl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetTcp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BridgeEthernetUdp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/EnumeratorEthernetUdp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BoardEthernetTcp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/BoardEthernetUdp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/SocketTcp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ethernet/SocketUdp.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/fpga/BridgeFpgaIrpli.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fpga/BridgeFpgaBase.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/DebugFrame.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/ErrorFrame.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/Frame.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameBase.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameForwarder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FramePool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frames/FrameQueue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/SerialPort.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/BridgeSerial.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/serial/BoardSerial.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/BoardUsb.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionCypress.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionCypressImpl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtek.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtekFlash.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/VendorExtensionRealtekI2c.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/boards/Board.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/boards/BoardGeneric.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/boards/BoardRemote.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/serial/SerialPortImpl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/serial/EnumeratorSerialImpl.cpp"
    )

if(${STRATA_TARGET_PLATFORM} STREQUAL Windows)
    set(PLATFORM_HEADERS
        ${PLATFORM_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EDirectShow.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeDirectShow.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeDirectShowDefinitions.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorDirectShow.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorDirectShowDefinitions.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/Win32Utils.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/SampleGrabberInterface.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/ComDefines.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/VidCap.hpp"
        )

    set(PLATFORM_SOURCES
        ${PLATFORM_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketTcpImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/ethernet/SocketUdpImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorDirectShow.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeDirectShow.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/Win32Utils.cpp"
        )

    list(APPEND STRATA_LIBRARY_DEPENDENCIES ws2_32 iphlpapi) # platform/ethernet
    list(APPEND STRATA_LIBRARY_DEPENDENCIES strmiids) # iids

    if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
        # specify Windows Vista, otherwise MinGW defaults to older Windows versions, disabling some current SDK features
        add_compile_definitions(_WIN32_WINNT=0x0600)

        # For mcd headers:
        # It seems that -Wno-cast-function-type is ignored due to-Wall
        # so -w is used to cancel -Wall
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    endif()
elseif(${STRATA_TARGET_PLATFORM} STREQUAL Linux)
    set(PLATFORM_HEADERS
        ${PLATFORM_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorLibUvc.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeV4l2.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/FrameV4l2.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/FramePoolV4l2.hpp"
        )

    set(PLATFORM_SOURCES
        ${PLATFORM_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/EnumeratorLibUvc.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/BridgeV4l2.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/FrameV4l2.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/${STRATA_TARGET_PLATFORM}/usb/FramePoolV4l2.cpp"
        )
endif()

if(${STRATA_TARGET_PLATFORM} MATCHES "(Linux)|(Macos)")
    set(PLATFORM_HEADERS
        ${PLATFORM_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketImpl.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketTcpImpl.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketUdpImpl.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/serial/SerialPortImplBase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/serial/EnumeratorSerialImplBase.hpp"
        )

    set(PLATFORM_SOURCES
        ${PLATFORM_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketTcpImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/ethernet/SocketUdpImpl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/serial/SerialPortImplBase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/unix/serial/EnumeratorSerialImplBase.cpp"
        )
endif()

find_package(Threads REQUIRED)
list(APPEND STRATA_LIBRARY_DEPENDENCIES ${CMAKE_THREAD_LIBS_INIT})


add_library(platform OBJECT ${PLATFORM_HEADERS} ${PLATFORM_SOURCES})
target_link_libraries_helper(platform rapidjson)

if(TARGET mcd_api)
    set(WIGGLER_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/BoardWiggler.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/BridgeWiggler.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/EnumeratorWiggler.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/MulticoreDebugger.hpp"
        )

    set(WIGGLER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/BoardWiggler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/BridgeWiggler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/EnumeratorWiggler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/wiggler/MulticoreDebugger.cpp"
        )

    target_sources(platform PRIVATE ${WIGGLER_HEADERS} ${WIGGLER_SOURCES})
    target_compile_definitions(platform PUBLIC STRATA_USE_MCD)
    list(APPEND STRATA_LIBRARY_OBJECTS $<TARGET_OBJECTS:mcd_api>)
    target_link_libraries_helper(platform mcd_api)
endif()


set(STRATA_LIBRARY_OBJECTS
    ${STRATA_LIBRARY_OBJECTS}
    $<TARGET_OBJECTS:platform>
    CACHE INTERNAL ""
    )
set(STRATA_LIBRARY_DEPENDENCIES
    ${STRATA_LIBRARY_DEPENDENCIES}
    CACHE INTERNAL ""
    )
