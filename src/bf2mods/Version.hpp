// Created by block on 2/10/2023.

#pragma once

#include <string_view>

namespace bf2mods::version {

	std::string_view GitVersion();
	std::string_view BuildTimestamp();

	// Captain Obvious: The Function
	bool IsDebug();

} // namespace bf2mods::version
