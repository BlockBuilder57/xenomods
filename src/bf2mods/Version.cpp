// Created by block on 2/10/2023.

#include "Version.hpp"
#include "gitversion.h"

namespace bf2mods::version {

	std::string_view GitVersion() {
		return fullTag;
	}
	std::string_view BuildTimestamp() {
		return __DATE__ " " __TIME__;
	}

	bool IsDebug() {
#if _DEBUG
		return true;
#else
		return false;
#endif
	}

} // namespace bf2mods::version