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
		static void* Hook(unsigned char* pBdat, unsigned char* pVarName, int idx) {
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
				.data = nullptr
			};

			std::vector<bf2mods::BdatOverrideBase*> applicableCallbacks;

			// reduce all applicable callbacks from global vector
			InsertIf(bf2mods::BdatOverride::Callbacks.begin(), bf2mods::BdatOverride::Callbacks.end(), applicableCallbacks, [&](auto* callback) {
				return callback->IsApplicable(access.sheet);
			});

			// run all applicable callbacks
			for(auto& callback : applicableCallbacks)
				(*callback)(access);

			if(access.data != nullptr)
				return access.data;

			return Orig(pBdat, pVarName, access.sheet.row);
		}
	};

} // namespace

namespace bf2mods {

	std::vector<bf2mods::BdatOverrideBase*> BdatOverride::Callbacks = {};

	void BdatOverride::RegisterCallback(bf2mods::BdatOverrideBase* override) {
		Callbacks.push_back(override);
	}

	void BdatOverride::Initialize() {
		g_Logger->LogDebug("Setting up Bdat overrides...");

		GetValHook::HookAt(Bdat::getVal);
	}

	BF2MODS_REGISTER_MODULE(BdatOverride);

} // namespace bf2mods