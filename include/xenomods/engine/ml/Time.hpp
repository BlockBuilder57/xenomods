// Created by block on 11/12/23.

#pragma once

#include <chrono>
#include <iomanip>

#include "xenomods/engine/mm/mtl/FixStr.hpp"

namespace ml {

	struct TimeInfo {
		union {
			std::uint32_t storage;
			struct [[gnu::packed]] {
				std::uint16_t millisecond : 14; // @0
				std::uint16_t second : 6;       // @14
				std::uint16_t minute : 6;       // @20
				std::uint16_t hour : 6;         // @26
			};
		} time;
		union {
			std::uint32_t storage;
			struct [[gnu::packed]] {
				std::uint16_t day : 5;         // @0
				std::uint16_t julian_date : 9; // @5
				std::uint16_t month : 4;       // @14
				std::uint16_t year : 14;       // @18
			};
		} date;
	};

	struct TimeUtil {
	   public:
		static bool getTime(TimeInfo& info);
		static bool getDateStr(mm::mtl::FixStr<64>& str);
		static bool getTimeStr(mm::mtl::FixStr<64>& str);

		// xenomods functions
		static std::string GetFullInfo(const TimeInfo& info) {
			return fmt::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:04d}", info.date.year, info.date.month, info.date.day, info.time.hour, info.time.minute, info.time.second, info.time.millisecond);
		}
		static unsigned long GetUnixTimestamp(const TimeInfo& info) {
			std::tm time {
				.tm_sec = info.time.second,
				.tm_min = info.time.minute,
				.tm_hour = info.time.hour,
				.tm_mday = info.date.day,
				.tm_mon = info.date.month - 1,
				.tm_year = info.date.year - 1900,
			};

			std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&time));
			return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
		}
	};

}