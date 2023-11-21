// Created by block on 11/12/23.

#pragma once

#include "xenomods/engine/mm/mtl/FixStr.hpp"

namespace ml {

	struct TimeInfo {
		union {
			std::uint32_t storage;
			struct [[gnu::packed]] {
				std::uint16_t day : 5;         // @0
				std::uint16_t julian_date : 9; // @5
				std::uint16_t month : 4;       // @14
				std::uint16_t year : 14;       // @18
			};
		} date;
		union {
			std::uint32_t storage;
			struct [[gnu::packed]] {
				std::uint16_t millisecond : 14; // @0
				std::uint16_t second : 6;       // @14
				std::uint16_t minute : 6;       // @20
				std::uint16_t hour : 6;         // @26
			};
		} time;
	};

	struct TimeUtil {
	   public:
		static bool getTime(TimeInfo& info);
		static bool getDateStr(mm::mtl::FixStr<64>& str);
		static bool getTimeStr(mm::mtl::FixStr<64>& str);
	};

}