#ifndef UTILS__SHELLCMD_H_
#define UTILS__SHELLCMD_H_
// *******************************************************************************
//
// Project: Utils.
//
// Module: Shell.
//
// *******************************************************************************

//! \file
//! \brief Shell command interface class.
//! \ingroup utils

// ******************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// Standard libraries.
#include <string_view>

// *****************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// *****************************************************************************

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

namespace Utils
{
	

// Forward declaration.
class Shell;


// Shell Command class.
// Contains methods to allow you to create commands that can be added to the Shell.
class ShellCmd final
{
public:
    using tFunction = void (*)(
        Utils::Shell* const aShell,
        const std::string_view aName,
        void* const aParam,
        const int aArgCount,
        const char* const aArgVal[]
    );

    [[nodiscard]] explicit constexpr ShellCmd(
        const char* const aName,
        const tFunction aFunction,
        void* const aParam = nullptr
    ) noexcept
        : mName{aName}
        , mFunction{aFunction}
        , mParam{aParam}
    {/*Ctor body*/}

    [[nodiscard]] auto GetName() const noexcept -> std::string_view {return mName;}

    void Execute(
        Shell* const aShell,
        const int aArgCount,
        const char* aArgVal[]
    ) const noexcept
    {
        if (nullptr != mFunction) (*mFunction)(aShell, mName, mParam, aArgCount, aArgVal);
    }

private:
    std::string_view mName;
    tFunction mFunction;
    void* mParam;
};


} // namespace Utils

// *****************************************************************************
//                            EXPORTED VARIABLES
// *****************************************************************************

// *****************************************************************************
//                                 EXTERNS
// *****************************************************************************

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
#endif // UTILS__SHELLCMD_H_
