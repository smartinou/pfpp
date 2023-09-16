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

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This module.
#include "utils/shell/Shell.h"

// Standard libraries.
#include <array>
#include <cstring>
#include <malloc.h>

// *****************************************************************************
//                       DEFINED CONSTANTS AND MACROS
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

namespace Utils
{


static constexpr std::size_t sLineSize {256};
static constexpr auto sTerminatedLineSize {sLineSize + 1};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

Shell::Shell() noexcept
{
    // Ctor body.
    AddHelpCmd();
    AddMemDispCmd();
    AddMemModCmd();
    AddHeapCmd();
}


void Shell::AddShellCmd(const ShellCmd aShellCmd) noexcept
{
    mCmds.insert(std::make_pair(aShellCmd.GetName(), aShellCmd));
}


void Shell::AddShellCmd(
    const char *aName,
    const Utils::ShellCmd::tFunction aFunction,
    void* const aParam
) noexcept
{
    mCmds.emplace(
        std::make_pair(
            std::string_view{aName},
            ShellCmd{aName, aFunction, aParam}
        )
    );
}


void Shell::Printf(const char* const aFormat, ...) noexcept
{
    std::array<char, sTerminatedLineSize> lLine {};

    va_list lArgs;
    va_start(lArgs, aFormat);
    std::vsnprintf(lLine.data(), lLine.size(), aFormat, lArgs);
    va_end(lArgs);

    printf(lLine.data());
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

void Shell::CmdTaskProc() noexcept
{
    mIsRunning = true;
    static constexpr auto sLineLen {80};
    char lCmdBuf[sLineLen + 1];

    Printf("Pleora Technologies Command Shell.\n");
    while (1) {
        Printf("PT>\n");
        scanf(" %80[^\n]", lCmdBuf);
        CmdLineDispatch(lCmdBuf);
    }

    // Should never get here.
    //vTaskDelete(nullptr);
}

#if 0
void Shell::PrintTaskProc() noexcept
{
    static std::array<char, sTerminatedLineSize> sLine {};
    while (1) {
        if (pdTRUE ==
            xQueueReceive(
                mCmdQueue.GetHandle(),
                static_cast<void*>(sLine.data()),
                portMAX_DELAY)
            )
        {
            print(sLine.data());
        }
    }

    // Should never get here.
    vTaskDelete(nullptr);
}
#endif

//
// Dispatches the command to the appropriate command handler.
//
void Shell::CmdLineDispatch(const char* const aCmdLine) noexcept
{
    static constexpr auto sMaxTokens {10};
    std::array<const char*, sMaxTokens> lTokens {};

    // Get the command and arguments.
    auto lTokenCount = decltype(lTokens.size()){0};
    lTokens[0] = std::strtok(const_cast<char*>(aCmdLine), " \n\r\t");
    while (lTokens[lTokenCount] && (lTokenCount < lTokens.size())) {
        ++lTokenCount;
        lTokens[lTokenCount] = std::strtok(nullptr, " \n\r\t");
    }

    if (0 == lTokenCount) {
        // Not even a command, expected as 1st token.
        return;
    }

    // Find the matching command in the table of commands.
    if (auto lCmd {mCmds.find(lTokens[0])}; lCmd != mCmds.cend()) {
        lCmd->second.Execute(this, lTokenCount - 1, &lTokens[1]);
    }
    else {
        Printf("Unknown command '%s'.\n", lTokens[0]);
    }
}


//
// Command Handlers
//

void Shell::AddHelpCmd() noexcept
{
    static constexpr ShellCmd::tFunction lCmd {
        [](
            Shell* const aShell,
            [[maybe_unused]] const std::string_view aName,
            [[maybe_unused]] void* const aParam,
            [[maybe_unused]] const int aArgc,
            [[maybe_unused]] const char* const aArgv[]
        ) noexcept
        {
            for (const auto& lCmd : aShell->mCmds) {
                aShell->Printf("%s\n", lCmd.first);
            }
        }
    };

    AddShellCmd("?", lCmd);
    AddShellCmd("help", lCmd);
}


void Shell::AddMemDispCmd() noexcept
{
    static constexpr auto lCmd {
        [](
            Shell* const aShell,
            const std::string_view aName,
            [[maybe_unused]] void* const aParam,
            const int aArgc,
            const char* const aArgv[]
        ) noexcept
        {
            // Remember previous values if none provided.
            static uint32_t sPrevAddr {0};
            auto lMemPtr {reinterpret_cast<uint32_t*>(sPrevAddr)};

            // Check for address argument.
            bool lShowHelp {false};
            if (aArgc > 0) {
                if (!sscanf(aArgv[0], "%lx", lMemPtr)) {
                    lShowHelp = true;
                }
            }

            // Check for length argument.
            static uint32_t sPrevLen {0};
            auto lLen {sPrevLen};
            if (aArgc > 1) {
                if (!sscanf(aArgv[1], "%lx", &lLen)) {
                    lShowHelp = true;
                }
                sPrevLen = lLen;
            }

            if (!lShowHelp) {
                // Display the memory.
                unsigned int lDispCnt {0};
                do {
                    static constexpr auto sColumns {4};
                    if (!(lDispCnt % sColumns)) {
                        aShell->Printf("\n0x%08x: ", reinterpret_cast<int>(lMemPtr));
                    }
                    aShell->Printf("0x%08x  ", *lMemPtr++);
                } while (++lDispCnt < lLen);
                aShell->Printf("\n");

                // Remember address and length for next time.
                sPrevAddr = reinterpret_cast<uint32_t>(lMemPtr);
            }

            if (lShowHelp) {
                aShell->Printf("format: %s [<addr>] [<size>]\n", aName);
            }
        }
    };

    AddShellCmd("md", lCmd);
}


void Shell::AddMemModCmd() noexcept
{
    static constexpr auto lCmd {
        [](
            Shell* const aShell,
            const std::string_view aName,
            [[maybe_unused]] void* const aParam,
            const int aArgc,
            const char* const aArgv[]
        ) noexcept
        {
            // Remember previous values if none provided.
            static uint32_t sPrevAddr {0};
            const auto lMemPtr {reinterpret_cast<uint32_t*>(sPrevAddr) - 1};

            // Check for address argument.
            bool lShowHelp {false};
            if (aArgc > 0) {
                if (!sscanf(aArgv[0], "%lx", lMemPtr)) {
                    lShowHelp = true;
                }
            }

            if (!lShowHelp) {
                // Display the memory.
                aShell->Printf("\n0x%08x: 0x%08x\n", reinterpret_cast<int>(lMemPtr), *lMemPtr);
                aShell->Printf("New value:");

                uint32_t lNewValue {};
                if (1 == scanf("%lx", &lNewValue)) {
                    // Write the value into memory.
                    *lMemPtr = lNewValue;
                    aShell->Printf("Wrote 0x%x to addr 0x%x.\n", lNewValue, reinterpret_cast<int>(lMemPtr));
                }
                else {
                    lShowHelp = true;
                }
                aShell->Printf("\n");
            }

            if (lShowHelp) {
                aShell->Printf("format: %s [<addr>]\n", aName);
            }
        }
    };

    AddShellCmd("mm", lCmd);
}


void Shell::AddHeapCmd() noexcept
{
    static constexpr auto lCmd {
        [](
            Shell* const aShell,
            const std::string_view aName,
            [[maybe_unused]] void* const aParam,
            const int aArgc,
            const char* const aArgv[]
        ) noexcept
        {
            bool lShowHelp {true};

            // Check for address argument.
            if (aArgc > 0) {
                if (!strcmp(aArgv[0], "stats")) {
                    malloc_stats();
                    lShowHelp = false;
                }
                else if (!strcmp(aArgv[0], "info")) {
                    aShell->Printf("arena:    %d\n", mallinfo().arena);
                    aShell->Printf("ordblks:  %d\n", mallinfo().ordblks);
                    aShell->Printf("smblks:   %d\n", mallinfo().smblks);
                    aShell->Printf("hblks:    %d\n", mallinfo().hblks);
                    aShell->Printf("hblkhd:   %d\n", mallinfo().hblkhd);
                    aShell->Printf("usmblks:  %d\n", mallinfo().usmblks);
                    aShell->Printf("fsmblks:  %d\n", mallinfo().fsmblks);
                    aShell->Printf("uordblks: %d\n", mallinfo().uordblks);
                    aShell->Printf("fordblks: %d\n", mallinfo().fordblks);
                    aShell->Printf("keepcost: %d\n", mallinfo().keepcost);

                    lShowHelp = false;
                }
            }

            if (lShowHelp) {
                aShell->Printf("format: %s [stats|info]\n", aName);
            }
        }
    };

    AddShellCmd("heap", lCmd);
}


} // namespace Utils

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
