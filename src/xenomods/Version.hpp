// Created by block on 2/10/2023.

#pragma once

#include <string_view>

namespace xenomods::version {

	std::string_view GitVersion();
	std::string_view BuildTimestamp();

	// Captain Obvious: The Function
	bool IsDebug();

} // namespace xenomods::version
