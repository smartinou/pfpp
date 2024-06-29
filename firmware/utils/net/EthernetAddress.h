#ifndef UTILS__NETETHERNETADDRESS_H_
#define UTILS__NETETHERNETADDRESS_H_
// *******************************************************************************
//
// Project: Network utilities.
//
// Module: Ethernet.
//
// *******************************************************************************

//! \file
//! \brief Ethernet class.
//! \ingroup utils_network

// ******************************************************************************
//
//        Copyright (c) 2015-2024, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

#include <cstdint>
#include <string>
#include <array>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Utils::Net
{


//! \brief Ethernet Address.
class EthernetAddress final
{
public:

    using EthernetArray = std::array<uint8_t, 6>;
    static constexpr EthernetArray sBroadcastVal{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    constexpr explicit EthernetAddress() noexcept
        : mAddress{0}
    {/*Ctor body.*/}

    constexpr explicit EthernetAddress(
        const uint8_t aB0, const uint8_t aB1, const uint8_t aB2, const uint8_t aB3, const uint8_t aB4, const uint8_t aB5
    ) noexcept
        : mAddress{aB0, aB1, aB2, aB3, aB4, aB5}
    {/*Ctor body.*/}

    constexpr explicit EthernetAddress(const EthernetArray &aBytes) noexcept
        : mAddress{aBytes}
    {/*Ctor body.*/}

    constexpr explicit EthernetAddress(const char* aStr) noexcept
        : EthernetAddress(aStr[0], aStr[1], aStr[2], aStr[3], aStr[4], aStr[5])
    {/*Ctor body.*/}

    constexpr explicit EthernetAddress(const uint64_t aValue) noexcept
        : EthernetAddress(
            static_cast<uint8_t>((aValue & 0x0000FF0000000000LL) >> 40),
            static_cast<uint8_t>((aValue & 0x000000FF00000000LL) >> 32),
            static_cast<uint8_t>((aValue & 0x00000000FF000000LL) >> 24),
            static_cast<uint8_t>((aValue & 0x0000000000FF0000LL) >> 16),
            static_cast<uint8_t>((aValue & 0x000000000000FF00LL) >>  8),
            static_cast<uint8_t>((aValue & 0x00000000000000FFLL) >>  0)
        )
    {/*Ctor body.*/}

    explicit EthernetAddress(const std::string& aStr)
        : EthernetAddress(aStr.c_str())
    {/*Ctor body.*/}

    [[nodiscard]] constexpr auto GetByte(const std::size_t aIndex) const noexcept -> uint8_t {return mAddress[aIndex];}

    void GetData(uint8_t* aByte) const noexcept;
    [[nodiscard]] auto ToString() const noexcept -> std::string;
    [[nodiscard]] auto GetValue() const noexcept -> uint64_t;

    [[nodiscard]] bool IsBroadcast() const noexcept;
    [[nodiscard]] bool IsMulticast() const noexcept;
    [[nodiscard]] bool IsUnicast() const noexcept;
    [[nodiscard]] bool IsValid() const noexcept;

private:
    void GetString(char* aOut) const noexcept;

    constexpr void Set(const unsigned int aIndex, const uint8_t aValue) noexcept
        {mAddress[aIndex] = aValue;}

    // Bytes to bytes copy is allowed.
    EthernetArray mAddress;
};

// ******************************************************************************
//                            EXPORTED VARIABLES
// ******************************************************************************

// ******************************************************************************
//                                 EXTERNS
// ******************************************************************************

// ******************************************************************************
//                            EXPORTED FUNCTIONS
// ******************************************************************************

static constexpr EthernetAddress sEthernetAddressBroadcast{EthernetAddress::sBroadcastVal};


} // namespace Utils::Net

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // UTILS__NETETHERNETADDRESS_H_