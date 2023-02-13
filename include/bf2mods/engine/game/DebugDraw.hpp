// Created by block on 2/12/2023.

#pragma once

#include <bf2mods/Utils.hpp>

namespace game {

	class DebugDraw {
	   public:
		enum class Group : std::uint32_t {

		};

		static void setEnable(Group group, bool enable);
	};

} // namespace game