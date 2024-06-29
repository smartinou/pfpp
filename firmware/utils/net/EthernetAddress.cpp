// *****************************************************************************
//
// Project: Network utilities
//
// Module: Ethernet.
//
// *****************************************************************************

//! \file
//! \brief Ethernet class.
//! \ingroup utils_network

// *****************************************************************************
//
//        Copyright (c) 2016-2024, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

#include <utils/net/EthernetAddress.h>

#include <algorithm>

// *****************************************************************************
//                      DEFINED CONSTANTS AND MACROS
// *****************************************************************************

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

// *****************************************************************************
//                            FUNCTION PROTOTYPES
// *****************************************************************************

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

namespace Utils::Net
{


void EthernetAddress::GetData(uint8_t* aOutByte) const noexcept
{
    for (const auto lByte : mAddress)
    {
        *aOutByte = lByte; ++aOutByte;
    }
    return;
}


///
/// \brief Returns a PtUtilsLib::String representation of the Ethernet address
///

auto EthernetAddress::ToString() const noexcept -> std::string
{
    std::array<char, 32> lStr{0};
    GetString(lStr.data());
    return std::string(lStr.data());
}


///
/// \brief Returns a 64-bits numerical representation of the address
///

auto EthernetAddress::GetValue() const noexcept -> uint64_t
{
    const uint64_t lValue{
        static_cast<uint64_t>(mAddress[0]) << ((6 - 1) * 8)
        | static_cast<uint64_t>(mAddress[1]) << ((5 - 1) * 8)
        | static_cast<uint64_t>(mAddress[2]) << ((4 - 1) * 8)
        | static_cast<uint64_t>(mAddress[3]) << ((3 - 1) * 8)
        | static_cast<uint64_t>(mAddress[4]) << ((2 - 1) * 8)
        | static_cast<uint64_t>(mAddress[5]) << ((1 - 1) * 8)
    };

    return lValue;
}


///
/// \brief Is the address a broadcast address?
///

bool EthernetAddress::IsBroadcast() const noexcept
{
    const bool lFound{
        std::any_of(
            std::cbegin(mAddress),
            std::cend(mAddress),
            [](const uint8_t aByte)
            {
                return (aByte != 0xFF);
            }
        )
    };

	return !lFound;
}


///
/// \brief Is the ethernet address multicast?
///

bool EthernetAddress::IsMulticast() const noexcept
{
    return (
        (mAddress[ 0 ] == 0x01) && 
        (mAddress[ 1 ] == 0x00) && 
        (mAddress[ 2 ] == 0x5e) && 
        ((mAddress[ 3 ] & 0x80) == 0x00)
    );
}


///
/// \brief Is the ethernet address unicast? (not broadcast)
///

bool EthernetAddress::IsUnicast() const noexcept
{
    return (!IsBroadcast() && !IsMulticast());
}


///
/// \brief Is the ethernet address valid?
///

bool EthernetAddress::IsValid() const noexcept
{
    const bool lFound{
        std::any_of(
            std::cbegin(mAddress),
            std::cend(mAddress),
            [](const uint8_t aByte)
            {
                return (aByte != 0x00);
            }
        )
    };

	return lFound;
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

///
/// \brief Get data as a string.
///

void EthernetAddress::GetString(char* aOut) const noexcept
{
    static constexpr auto sDigits{"0123456789abcdef"};
    for (const auto lByte : mAddress)
    {
        *aOut = sDigits[lByte >> 4]; ++aOut;
        *aOut = sDigits[lByte & 0x0F]; ++aOut;
        *aOut = ':'; ++aOut;
    }

	--aOut;
	*aOut = '\0';
    return;
}


} // namespace Utils::Net

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
