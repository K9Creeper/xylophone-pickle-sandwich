/// ----------
/// serial-com.hpp
/// @brief This file declares the needed function(s) to setup the COM1 serial port,
/// aswell as debuging through said port.

#pragma once

#define COM1_PORT_ADDRESS 0x3F8

namespace Kernel{
    namespace Misc{
        extern void Serial_Com_Init();
    }
}