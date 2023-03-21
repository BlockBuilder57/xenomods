// Created by block on 2/10/2023.

#pragma once

#include <string_view>

#include "Utils.hpp"
#include "fmt/core.h"
#include "gitversion.h"
#include "xenomods/engine/ml/ProcDesktop.hpp"

namespace xenomods::version {

	enum class GameType {
		Invalid,
		BF2,
		IRA,
		BFSW,
		BF3
	};

	struct SemVer {
		std::uint8_t major {};
		std::uint8_t minor {};
		std::uint8_t patch {};

		inline bool IsValid() {
			return major != 0 && minor != 0 && patch != 0;
		}
	};

	inline const char* BuildGitVersion() { return fullTag; }
	inline const char* BuildTimestamp() { return __DATE__ " " __TIME__; }
	const GameType BuildGame =
#if XENOMODS_CODENAME(bf2)
	GameType::BF2;
#elif XENOMODS_CODENAME(ira)
	GameType::IRA;
#elif XENOMODS_CODENAME(bfsw)
	GameType::BFSW;
#elif XXENOMODS_CODENAME(bf3)
	GameType::BF3;
#endif
	inline bool BuildIsDebug() {
#if _DEBUG
		return true;
#else
		return false;
#endif
	}

	unsigned long RuntimeProgramID();
	GameType RuntimeGame();
	SemVer RuntimeVersion();
	inline const char* RuntimeBuildRevision() { return ml::ProcDesktop::getBuildRevision()->buffer; }

} // namespace xenomods::version

template<>
struct fmt::formatter<xenomods::version::GameType> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(xenomods::version::GameType type, FormatContext& ctx) {
		std::string_view name;

		// clang-format off
		switch(type) {
			using enum xenomods::version::GameType;

			case BF2: name = "Xenoblade 2"; break;
			case IRA: name = "Xenoblade 2: Torna"; break;
			case BFSW: name = "Xenoblade: Definitive Edition"; break;
			case BF3: name = "Xenoblade 3"; break;
			case Invalid: default: name = "Invalid game"; break;
		}
		// clang-format on

		return formatter<std::string_view>::format(name, ctx);
	}
};

template<>
struct fmt::formatter<xenomods::version::SemVer> : fmt::formatter<std::string> {
	template<typename FormatContext>
	inline auto format(const xenomods::version::SemVer& semver, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), FMT_STRING("{}.{}.{}"), semver.major, semver.minor, semver.patch);
	}
};