#ifndef UTILS__SHELL_H_
#define UTILS__SHELL_H_
// *******************************************************************************
//
// Project: Utils.
//
// Module: Shell.
//
// *******************************************************************************

//! \file
//! \brief Shell interface class.
//! \ingroup utils

// ******************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

// This module.
#include "utils/shell/ShellCmd.h"

// Standard libraries.
#include <cstdarg>
#include <cstdio>
#include <map>
#include <string_view>

// *****************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// *****************************************************************************

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

namespace Utils
{


class Shell final
{
public:
    [[nodiscard]] explicit Shell() noexcept;

    void Printf(const char* aFormat, ...) noexcept;
    void AddShellCmd(Utils::ShellCmd aShellCmd) noexcept;
    void AddShellCmd(
        const char* aName,
        Utils::ShellCmd::tFunction aFunction,
        void* aParam = nullptr
    ) noexcept;
    void CmdLineDispatch(const char* aCmdLine) noexcept;

private:

    void AddHelpCmd() noexcept;
    void AddMemDispCmd() noexcept;
    void AddMemModCmd() noexcept;
    void AddHeapCmd() noexcept;

    // Commands table.
    std::map<std::string_view, ShellCmd> mCmds;
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
#endif // UTILS__SHELL_H_
