// Created by block on 2/10/2023.

#pragma once

#include <string_view>

#include "Utils.hpp"
#include "fmt/core.h"
#include "gitversion.h"
#include "skylaunch/utils/cpputils.hpp"
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

		static const SemVer v2_0_0;
		static const SemVer v2_1_0;

		inline bool IsValid() {
			return major != 0 && minor != 0 && patch != 0;
		}

		inline bool operator==(const SemVer& other) const {
			return this->major == other.major && this->minor == other.minor && this->patch == other.patch;
		}
		inline bool operator!=(const SemVer& other) const { return !(*this == other); }

		inline bool operator<(const SemVer& other) const {
			return this->major <= other.major && this->minor <= other.minor && this->patch < other.patch;
		}
		inline bool operator>(const SemVer& other) const { return other < *this; }
		inline bool operator<=(const SemVer& other) const { return !(other > *this); }
		inline bool operator>=(const SemVer& other) const { return !(other < *this); }
	};

	const char* BuildGitVersion();
	const char* BuildTimestamp();

	// returns a string like "Xenomods 1234567~"
	const char* XenomodsVersion();
	// returns a string like "Xenomods 1234567~ (debug) [???]"
	const char* XenomodsFullVersion();

	const GameType BuildGame =
#if XENOMODS_CODENAME(bf2)
	GameType::BF2;
#elif XENOMODS_CODENAME(ira)
	GameType::IRA;
#elif XENOMODS_CODENAME(bfsw)
	GameType::BFSW;
#elif XENOMODS_CODENAME(bf3)
	GameType::BF3;
#endif
	const bool BuildIsDebug =
#if _DEBUG
	true;
#else
	false;
#endif

	unsigned long RuntimeProgramID();
	GameType RuntimeGame();
	SemVer RuntimeVersion();
#if !XENOMODS_CODENAME(bf3)
	inline const char* RuntimeBuildRevision() { return ml::ProcDesktop::getBuildRevision()->buffer; }
#else
	inline const char* RuntimeBuildRevision() {
		// ml::_dsk::s_revision
		if (RuntimeVersion() == SemVer::v2_0_0)
			return reinterpret_cast<mm::mtl::FixStr<128>*>(skylaunch::utils::g_MainTextAddr + 0x1b6f500)->buffer;
		else if (RuntimeVersion() == SemVer::v2_1_0)
			return reinterpret_cast<mm::mtl::FixStr<128>*>(skylaunch::utils::g_MainTextAddr + 0x1b70500)->buffer;

		return nullptr;
	}
#endif

} // namespace xenomods::version

template<>
struct fmt::formatter<xenomods::version::GameType> : fmt::formatter<std::string_view> {
	enum class ParseType {
		Default,
		Shorter,
		Codename
	};
	ParseType parsetype = ParseType::Default;

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();

		if (it != end) {
			// if there's literally any format
			if (*it == 's' || *it == 'S')
				parsetype = ParseType::Shorter;
			else if (*it == 'c' || *it == 'C')
				parsetype = ParseType::Codename;
		}

		return ++it;
	}

	template<typename FormatContext>
	inline auto format(xenomods::version::GameType type, FormatContext& ctx) {
		std::string_view name;
		using enum xenomods::version::GameType;

		// clang-format off
		if (parsetype == ParseType::Default) {
			switch(type) {
				case BF2: name = "Xenoblade 2"; break;
				case IRA: name = "Xenoblade 2: Torna"; break;
				case BFSW: name = "Xenoblade: Definitive Edition"; break;
				case BF3: name = "Xenoblade 3"; break;
				default: name = "Invalid game"; break;
			}
		}
		else if (parsetype == ParseType::Shorter) {
			switch(type) {
				case BF2: name = "XB2"; break;
				case IRA: name = "Torna"; break;
				case BFSW: name = "XBDE"; break;
				case BF3: name = "XB3"; break;
				default: name = "Invalid"; break;
			}
		}
		else if (parsetype == ParseType::Codename) {
			switch(type) {
				case BF2: name = "bf2"; break;
				case IRA: name = "ira"; break;
				case BFSW: name = "bfsw"; break;
				case BF3: name = "bf3"; break;
				default: name = "???"; break;
			}
		}
		// clang-format on

		return formatter<std::string_view>::format(name, ctx);
	}
};

template<>
struct fmt::formatter<xenomods::version::SemVer> : fmt::formatter<std::string> {
	template<typename FormatContext>
	inline auto format(const xenomods::version::SemVer& semver, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), "{}.{}.{}", semver.major, semver.minor, semver.patch);
	}
};