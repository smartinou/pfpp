#ifndef UTILS__NETIPADDRESS_H_
#define UTILS__NETIPADDRESS_H_
// *******************************************************************************
//
// Project: Network utilities.
//
// Module: IPAddress.
//
// *******************************************************************************

//! \file
//! \brief IPAddress class.
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

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Utils::Net
{


// \brief IP Address.
class IPAddress final
{
public:

    static constexpr uint32_t sBroadcastVal{0xFFFFFFFF};

    constexpr explicit IPAddress() noexcept
        : mValue{0}
        , mIsAny{true}
    {/*Ctor body.*/}

    constexpr explicit IPAddress(const uint8_t aB0, const uint8_t aB1, const uint8_t aB2, const uint8_t aB3) noexcept
        : mValue{
            aB0
            | static_cast<uint32_t>(aB1 << 8)
            | static_cast<uint32_t>(aB2 << 16)
            | static_cast<uint32_t>(aB3 << 24)
        }
        , mIsAny{false}
    {/*Ctor body.*/}

    constexpr explicit IPAddress(const uint32_t aIn) noexcept
        : mValue{aIn}
        , mIsAny{false}
    {/*Ctor body.*/}

	[[nodiscard]] auto GetByte(std::size_t aIndex) const noexcept -> uint8_t;
    void SetByte(std::size_t aIndex, uint8_t aByte) noexcept;
    [[nodiscard]] auto GetValue() const noexcept -> uint32_t {return mValue;}

    [[nodiscard]] auto ToString() const noexcept -> std::string;
    
    [[nodiscard]] bool IsBroadcast() const noexcept {return (mValue == sBroadcastVal);}
    [[nodiscard]] bool IsMulticast() const noexcept;
    [[nodiscard]] bool IsUnicast() const noexcept {return ((!IsBroadcast()) && (!IsMulticast()));}
    [[nodiscard]] bool IsAny() const noexcept {return mIsAny;}
    [[nodiscard]] bool IsValid() const noexcept {return ((mValue != 0) || mIsAny);}
    [[nodiscard]] bool IsValidMask() const noexcept;

    void SetAny(const bool aEnable = true) noexcept;
    void SetBroadcast() noexcept {mValue = sBroadcastVal;}

private:
    void GetString(char* aOut) const;

    uint32_t mValue;
    bool mIsAny;
};

// ******************************************************************************
//                            EXPORTED VARIABLES
// ******************************************************************************

static constexpr IPAddress sIPAddressBroadcast{IPAddress::sBroadcastVal};


} // namespace Utils::Net

// ******************************************************************************
//                                 EXTERNS
// ******************************************************************************

// ******************************************************************************
//                            EXPORTED FUNCTIONS
// ******************************************************************************

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // UTILS__NETIPADDRESS_H_
