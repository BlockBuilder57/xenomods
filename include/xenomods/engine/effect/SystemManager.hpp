// Created by block on 8/26/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_OLD_ENGINE

namespace effect {

	class SystemManager {
		typedef void (*string_func)(const char*); // type for conciseness

		static void setUserOutputString(string_func debugString, string_func warnString);
	};

} // namespace effect

#endif