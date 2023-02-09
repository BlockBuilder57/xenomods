//
// Created by block on 2/7/2023.
//

#include "BdatOverride.hpp"

#include <bf2mods/DebugWrappers.hpp>
#include <bf2mods/HidInput.hpp>
#include <bf2mods/Logger.hpp>
#include <bf2mods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "bf2mods/engine/bdat/Bdat.hpp"

namespace {

	template<class InputIt, class OutputContainer, class UnaryOp>
	constexpr OutputContainer& InsertIf(InputIt begin, InputIt end, OutputContainer& container, UnaryOp op) {
		while(begin != end)
			if(auto obj = *begin++; op(obj))
				container.push_back(obj);

		return container;
	}

	struct GetValHook : skylaunch::hook::Trampoline<GetValHook> {
		static unsigned long Hook(unsigned char* pBdat, unsigned char* pVarName, int idx) {
			auto sheetName = std::string_view { Bdat::getSheetName(pBdat) };
			auto memberName = std::string_view { reinterpret_cast<char*>(pBdat + *reinterpret_cast<short*>(pVarName + 4)) };
			//bf2mods::g_Logger->LogInfo("[Bdat] {}/{}:{}", sheetName, memberName, idx);

			bf2mods::BdatOverrideBase::Access access {
				.sheet = {
					.buffer = pBdat,
					.name = sheetName,
					.member = memberName,
					.row = static_cast<unsigned short>(idx)
				},
				.data = 0
			};

			std::vector<bf2mods::BdatOverrideBase*> applicableCallbacks;

			// reduce all applicable callbacks from global vector
			InsertIf(bf2mods::BdatOverride::Callbacks.begin(), bf2mods::BdatOverride::Callbacks.end(), applicableCallbacks, [&](auto* callback) {
				return callback->IsApplicable(access.sheet);
			});

			// run all applicable callbacks
			for(auto& callback : applicableCallbacks)
				(*callback)(access);

			if(access.data != 0)
				return access.data;

			return Orig(pBdat, pVarName, access.sheet.row);
		}
	};

	struct TomlBdatOverride : bf2mods::BdatOverrideBase {
		[[nodiscard]] bool IsApplicable(SheetData& sheet) const override {
			return bf2mods::BdatOverride::TOMLTable[sheet.name].is_table();
		}

		void operator()(Access& access) override {
			std::string unfortunateConversion = fmt::format("{}", access.sheet.row);
			toml::table* rowTable = bf2mods::BdatOverride::TOMLTable[access.sheet.name][unfortunateConversion].as_table();
			if (rowTable == nullptr)
				return; // no table for the row

			// we can declare a fallback for unchanged values
			ushort rowPre = access.sheet.row;
			auto fallbackRow = rowTable->get("_fallback");
			if (fallbackRow != nullptr) {
				access.sheet.row = fallbackRow->value_or(access.sheet.row);
				//bf2mods::g_Logger->LogWarning("{}/{}:{} requested fallback to {}", access.sheet.name, access.sheet.member, rowPre, access.sheet.row);
			}

			auto memberNode = rowTable->get(access.sheet.member);
			if (memberNode == nullptr) {
				//bf2mods::g_Logger->LogDebug("no value for {}/{}:{}", access.sheet.name, access.sheet.member, rowPre);
				return; // no value for this member
			}

			using enum Bdat::ValueType;

			auto memberPtr = Bdat::getMember(access.sheet.buffer, access.sheet.member.data());
			auto type = Bdat::getVarType(access.sheet.buffer, memberPtr);

			switch(type) {
				case Bdat::kUByte:
					access.data = static_cast<unsigned long>(memberNode->value<std::uint64_t>().value());
					break;
				case Bdat::kUInt16:
					access.data = static_cast<unsigned long>(memberNode->value<std::uint16_t>().value());
					break;
				case Bdat::kUInt32:
					access.data = static_cast<unsigned long>(memberNode->value<std::uint32_t>().value());
					break;
				case Bdat::kSByte:
					access.data = static_cast<unsigned long>(memberNode->value<std::int64_t>().value());
					break;
				case Bdat::kInt16:
					access.data = static_cast<unsigned long>(memberNode->value<std::int16_t>().value());
					break;
				case Bdat::kInt32:
					access.data = static_cast<unsigned long>(memberNode->value<std::int32_t>().value());
					break;
				case Bdat::kString:
					access.data = reinterpret_cast<unsigned long>(memberNode->value<const char*>().value());
					break;
				case Bdat::kFloat:
					access.data = static_cast<unsigned long>(memberNode->value<float>().value());
					break;
				default:
					break;
			}
		};
	};

	static auto& TomlOverride() {
		static TomlBdatOverride gOverride;
		return gOverride;
	}

} // namespace

namespace bf2mods {

	std::vector<bf2mods::BdatOverrideBase*> BdatOverride::Callbacks = {};
	toml::table BdatOverride::TOMLTable = {};

	void BdatOverride::RegisterCallback(bf2mods::BdatOverrideBase* override) {
		Callbacks.push_back(override);
	}

	void BdatOverride::Initialize() {
		g_Logger->LogDebug("Setting up Bdat overrides...");
		LoadFromFile();

		GetValHook::HookAt(Bdat::getVal);

		RegisterCallback(&TomlOverride());
	}

	void BdatOverride::Update() {
		if(GetPlayer(2)->InputDownStrict(Keybind::RELOAD_CONFIG)) {
			LoadFromFile();
		}
	}

	void BdatOverride::LoadFromFile() {
		auto path = fmt::format("sd:/config/bf2mods/{}/bdatOverride.toml", BF2MODS_CODENAME_STR);
		toml::parse_result res = toml::parse_file(path);

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogDebug("No Bdat override file could be found ({})", error.description());
			return;
		}

		TOMLTable = std::move(res).table();
	}

	BF2MODS_REGISTER_MODULE(BdatOverride);

} // namespace bf2mods