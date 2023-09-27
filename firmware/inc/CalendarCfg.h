#ifndef PFPP__CALENDARCFG_H_
#define PFPP__CALENDARCFG_H_
// *******************************************************************************
//
// Project: PFPP
//
// Module: Configuration.
//
// *******************************************************************************

//! \file
//! \brief Calendar configuration.
//! \ingroup apps

// ******************************************************************************
//
//        Copyright (c) 2015-2023, Martin Garon, All rights reserved.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

// Standard libraries.
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

template <typename T>
class DailyPlanner final
{
public:
    [[maybe_unused]] auto AddEntry(const T aEntry) -> bool
    {
        if (const auto lFind {
                std::find(
                    mEntries.cbegin(),
                    mEntries.cend(),
                    aEntry
                )
            };
            lFind == mEntries.cend()
        )
        {
            mEntries.push_back(aEntry);
            std::sort(mEntries.begin(), mEntries.end());
            return true;
        }
        return false;
    }

    [[maybe_unused]] auto DeleteEntry(const T aEntry) noexcept -> bool
    {
        const auto lErased {std::erase(mEntries, aEntry)};
        return (lErased > 0);
    }

    void DeleteAllEntries() noexcept {mEntries.clear();}

    [[nodiscard]] auto GetFirstEntry() const noexcept -> std::optional<T>
    {
        return mEntries.size() ? std::optional<T> {mEntries.at(0)} : std::nullopt;
    }

    [[nodiscard]] auto GetNextEntry(
        const T aEntry,
        const bool aWrapAround = true
    ) const noexcept -> std::optional<T>
    {
        // We must assume a request for an entry that doesn't exist.
        // so we search for the 1st entry greater in value than requested.
        if (const auto lIt {
                std::find_if(
                    mEntries.cbegin(),
                    mEntries.cend(),
                    [aEntry](const auto aVectorEntry)
                    {
                        return (aEntry < aVectorEntry);
                    }
                )
            };
            lIt != mEntries.cend()
        )
        {
            // Return found value.
            return *lIt;
        }
        else if (aWrapAround) {
            // Return 1st entry in the vector.
            // [MG] CONSIDER std::optional<T>(std::in_place{})
            return GetFirstEntry();
        }

        return std::nullopt;
    }

private:
    std::vector<T> mEntries;
};


template<typename T>
class WeeklyPlanner final
{
public:
    using Weekday = std::chrono::weekday;
    struct DayAndTime
    {
        Weekday mWeekday{};
        T mTime{};
    };

    [[maybe_unused]] auto AddEntry(
        const T aTimeEntry,
        const Weekday aWeekday
    ) -> bool
    {
        auto& lDailyPlanner {mPlanner.at(aWeekday.c_encoding())};
        return lDailyPlanner.AddEntry(aTimeEntry);
    }

    [[maybe_unused]] auto DeleteEntry(
        const T aTimeEntry,
        const Weekday aWeekday
    ) noexcept -> bool
    {
        auto& lDailyPlanner {mPlanner.at(aWeekday.c_encoding())};
        return lDailyPlanner.DeleteEntry(aTimeEntry);
    }

    void DeleteAllEntries() noexcept
    {
        for (auto& lDailyPlanner : mPlanner) {lDailyPlanner.DeleteAllEntries();}
    }

    [[nodiscard]] auto GetNextEntry(
        const T aTimeEntry,
        const Weekday aWeekday
    ) const noexcept -> std::optional<struct DayAndTime>
    {
        // Look for next entry in current weekday.
        const auto& lDailyPlanner {mPlanner.at(aWeekday.c_encoding())};
        const auto lNextEntry {lDailyPlanner.GetNextEntry(aTimeEntry, false)};
        if (lNextEntry) {
            return DayAndTime{aWeekday, lNextEntry.value()};
        }

        // Catch the first entry starting next following day.
        return GetFirstEntry(aWeekday);
    }

private:
    [[nodiscard]] auto GetFirstEntry(const Weekday aWeekday) const noexcept
        -> std::optional<struct DayAndTime>
    {
        Weekday lWeekday{aWeekday};
        for (auto lIx{0}; lIx < mPlanner.size(); ++lIx) {
            ++lWeekday;
            const auto& lDailyPlanner {mPlanner.at(lWeekday.c_encoding())};
            const auto lFirstEntry {lDailyPlanner.GetFirstEntry()};
            if (lFirstEntry) {
                return DayAndTime{aWeekday, lFirstEntry.value()};
            }
        }

        return std::nullopt;
    }

    static constexpr auto sDaysPerWeek{7};
    std::array<DailyPlanner<T>, sDaysPerWeek> mPlanner;
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

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // PFPP__CALENDARCFG_H_
