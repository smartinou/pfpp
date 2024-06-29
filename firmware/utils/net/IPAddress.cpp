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

#include "utils/net/IPAddress.h"

#include <array>
#include <bitset>

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


///
/// \brief Data accessor.
///

uint8_t IPAddress::GetByte(const std::size_t aIndex) const noexcept
{
	return (reinterpret_cast<const uint8_t*>(&mValue)[aIndex]);
}


void IPAddress::SetByte(const std::size_t aIndex, const uint8_t aByte) noexcept
{
    reinterpret_cast<uint8_t*>(&mValue)[aIndex] = aByte;
    return;
}


///
/// \brief Returns a PtUtilsLib::String representation of the IP address.
///

std::string IPAddress::ToString() const noexcept
{
    std::array<char, 32> lStr{0};
    GetString(lStr.data());
    return std::string(lStr.data());
}


///
/// \brief Checks if the IP address is in the multicast range.
///

bool IPAddress::IsMulticast() const noexcept
{
    return ((mValue & 0x000000F0UL) == 0x000000E0UL);
}


///
/// \brief Returns true if the IP address contains a valid subnet mask
///

bool IPAddress::IsValidMask() const noexcept
{
    bool lFoundOne = false;
    bool lFoundZero = false;
    static constexpr auto sSize{sizeof(uint32_t) * 8};
    std::bitset<sSize> lMask {mValue};
    for (auto i = decltype(sSize){0}; i < sSize; ++i)
    {
        if (lMask.test(sSize - 1))
        {
            // Found a one
            lFoundOne = true;
            if (lFoundZero)
            {
                return false;
            }
        } 
        else
        {
            lFoundZero = true;
        }
        lMask <<= 1;
    }

    return lFoundOne && lFoundZero;
}


///
/// \brief Sets as the 'any' address.
///

void IPAddress::SetAny(const bool aEnable) noexcept
{
    mIsAny = aEnable;
    mValue = 0;
    return;
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

///
/// \brief Convert the address to a string.
///
/// \param aOut The method store the string here.
///

void IPAddress::GetString(char* aOut) const
{
	static constexpr auto sSize{4 * sizeof(uint8_t)};
	for (auto i = decltype(sSize){0}; i < sSize; ++i)
    {
		unsigned short lByte = GetByte(i);
		if (lByte >= 100)
        {
			*aOut = static_cast<char>('0' + (lByte / 100)); ++aOut;
			lByte %= 100;
			*aOut = static_cast<char>('0' + (lByte / 10)); ++aOut;
			lByte %= 10;
			*aOut = static_cast<char>('0' + lByte); ++aOut;
		}
        else if (lByte >= 10)
        {
			*aOut = static_cast<char>('0' + (lByte / 10)); ++aOut;
			lByte %= 10;
			*aOut = static_cast<char>('0' + lByte); ++aOut;
		}
        else
        {
			*aOut = static_cast<char>('0' + lByte); ++aOut;
		}

		*aOut = '.'; ++aOut;
	}

	--aOut;
	*aOut = '\0';
    return;
}


} // namespace Utils::Net

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
