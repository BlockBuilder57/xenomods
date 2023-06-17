//
// Created by block on 2/7/2023.
//

#include "BdatOverride.hpp"

#include "xenomods/engine/bdat/Bdat.hpp"

namespace {

	struct GetValHook : skylaunch::hook::Trampoline<GetValHook> {
		static unsigned long Hook(unsigned char* pBdat, unsigned char* pVarName, int idx) {
			if (xenomods::BdatOverride::Callbacks.empty())
				return Orig(pBdat, pVarName, idx);

			auto sheetName = Bdat::getSheetName(pBdat);
			auto memberName = reinterpret_cast<char*>(pBdat + *reinterpret_cast<short*>(pVarName + 4));
			//xenomods::g_Logger->LogInfo("[Bdat] {}/{}:{}", sheetName, memberName, idx);

			//xenomods::BdatOverride::HotPath[sheetName]++;

			auto& skips = xenomods::GetState().config.bdatSkipOverrides;
			for (auto& it : skips) {
				if (it.starts_with(sheetName)) {
					//xenomods::g_Logger->LogInfo("[Bdat] Skipping {} ({})", sheetName, it);
					return Orig(pBdat, pVarName, idx);
				}
			}

			xenomods::BdatOverrideBase::Access access {
				.sheet = {
					.buffer = pBdat,
					.name = sheetName,
					.member = memberName,
					.row = static_cast<unsigned short>(idx)
				},
				.data = 0
			};

			// run all applicable callbacks
			for(xenomods::BdatOverrideBase* callback : xenomods::BdatOverride::Callbacks) {
				if (callback->IsApplicable(access.sheet))
					(*callback)(access);
			}

			if(access.data != 0)
				return access.data;

			return Orig(pBdat, pVarName, access.sheet.row);
		}
	};

	struct IDCountOverride : skylaunch::hook::Trampoline<IDCountOverride> {
		static unsigned short Hook(unsigned char* pBdat) {
			auto sheet = reinterpret_cast<Bdat::BdatSheet*>(pBdat);
			auto sheetName = Bdat::getSheetName(pBdat);
			unsigned short result = Orig(pBdat);

			if (xenomods::BdatOverride::SheetMaxIDs.contains(sheetName)) {
				unsigned short maxId = xenomods::BdatOverride::SheetMaxIDs[sheetName] - sheet->idTop + 1;
				if (maxId > result)
					result = maxId;
			}

			return result;
		}
	};

	struct IDEndOverride : skylaunch::hook::Trampoline<IDEndOverride> {
		static unsigned short Hook(unsigned char* pBdat) {
			auto sheetName = Bdat::getSheetName(pBdat);
			unsigned short result = Orig(pBdat);

			if (xenomods::BdatOverride::SheetMaxIDs.contains(sheetName)) {
				unsigned short maxId = xenomods::BdatOverride::SheetMaxIDs[sheetName];
				if (maxId > result)
					result = maxId;
			}

			return result;
		}
	};

	struct TomlBdatOverride : xenomods::BdatOverrideBase {
		[[nodiscard]] bool IsApplicable(SheetData& sheet) const override {
			if (xenomods::BdatOverride::SheetMaxIDs.empty())
				return false;

			return xenomods::BdatOverride::SheetMaxIDs.contains(sheet.name);
		}

		void operator()(Access& access) override {
			std::string unfortunateConversion = std::to_string(access.sheet.row);

			toml::table* rowTable = xenomods::BdatOverride::TOMLTable[access.sheet.name][unfortunateConversion].as_table();
			if (rowTable == nullptr)
				return; // no table for the row

			// we can declare a fallback for unchanged values
			//ushort rowPre = access.sheet.row;
			auto fallbackRow = rowTable->get("_fb");
			if (fallbackRow != nullptr) {
				access.sheet.row = fallbackRow->value_or(access.sheet.row);
				//xenomods::g_Logger->LogWarning("{}/{}:{} requested fallback to {}", access.sheet.name, access.sheet.member, rowPre, access.sheet.row);
			}

			auto memberNode = rowTable->get(access.sheet.member);
			if (memberNode == nullptr) {
				//xenomods::g_Logger->LogDebug("no value for {}/{}:{}", access.sheet.name, access.sheet.member, rowPre);
				return; // no value for this member
			}

			using enum Bdat::ValueType;

			auto memberPtr = Bdat::getMember(access.sheet.buffer, access.sheet.member);
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

namespace xenomods {

	std::vector<xenomods::BdatOverrideBase*> BdatOverride::Callbacks = {};
	toml::table BdatOverride::TOMLTable = {};
	std::unordered_map<std::string_view, unsigned short> BdatOverride::SheetMaxIDs = {};
	//std::unordered_map<std::string_view, unsigned long> BdatOverride::HotPath = {};

	void BdatOverride::RegisterCallback(xenomods::BdatOverrideBase* override) {
		Callbacks.push_back(override);
	}

	void BdatOverride::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up Bdat overrides...");

		GetValHook::HookAt(Bdat::getVal);
		IDCountOverride::HookAt(Bdat::getIdCount);
		IDEndOverride::HookAt(Bdat::getIdEnd);

		LoadFromFile();
	}

	void BdatOverride::OnConfigUpdate() {
		LoadFromFile();

		/*unsigned long total = 0;
		std::map<unsigned long, std::string_view, std::greater<>> sorted;
		for (auto& it : HotPath) {
			sorted[it.second] = it.first;
			total += it.second;
		}
		for (auto& it : sorted) {
			g_Logger->LogDebug("{}: {:.2f}% ({})", it.second, ((float)it.first / total) * 100.f, it.first);
		}
		HotPath.clear();*/
	}

	void BdatOverride::LoadFromFile() {
		auto path = fmt::format("sd:/config/xenomods/{}/bdatOverride.toml", XENOMODS_CODENAME_STR);
		toml::parse_result res = toml::parse_file(path);

		SheetMaxIDs.clear();
		TOMLTable.clear();
		std::remove(Callbacks.begin(), Callbacks.end(), &TomlOverride());

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogDebug("Bdat override file error: ({})", error.description());
			return;
		}

		TOMLTable = std::move(res).table();
		TOMLTable.for_each([&](auto& sheetKey, auto& sheetEl) {
			if constexpr(toml::is_table<decltype(sheetEl)>) {
				toml::table sheetName = sheetEl;
				unsigned short maxRow = 0;

				sheetName.for_each([&](auto& rowKey, auto& rowEl) {
					if (toml::is_table<decltype(rowEl)>) {
						int attempt = std::atoi(rowKey.data());
						if (attempt > 0 && maxRow < attempt)
							maxRow = attempt;
					}
				});

				SheetMaxIDs[sheetKey.data()] = maxRow;
				//g_Logger->LogDebug("Adding {} with {} max", sheetKey.data(), maxRow);
			}
		});

		if (!TOMLTable.empty()) {
			RegisterCallback(&TomlOverride());
			g_Logger->ToastInfo("configbdat", "Loaded Bdat override!");
		} else {
			g_Logger->ToastDebug("configbdat", "(Bdat override empty)");
		}
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(BdatOverride);
#endif

} // namespace xenomods