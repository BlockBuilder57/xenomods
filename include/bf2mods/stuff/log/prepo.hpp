#pragma once

#include "../utils/util.hpp"
#include "nn/fs.h"
#include "nn/prepo.h"
#include "skyline/utils/cpputils.hpp"

namespace log {

	static std::string s_gameStr;

	// prepo logging
	void logPrepo(nn::prepo::PlayReport* p_reportObj) {
		auto outDirPath = "sd:/prepo/" + s_gameStr;
		nn::fs::CreateDirectory("sd:/prepo");
		nn::fs::CreateDirectory(outDirPath.c_str());
		nn::time::PosixTime time;
		nn::time::StandardUserSystemClock::GetCurrentTime(&time);
		std::string reportOutputPath = "sd:/prepo/" + s_gameStr + "/";
		reportOutputPath += std::to_string(time.time) + "_";
		reportOutputPath += std::to_string(svcGetSystemTick()) + "_";
		reportOutputPath += std::string((char*)&p_reportObj->m_EventName) + ".bin";

		skyline::utils::writeFile(reportOutputPath.c_str(), 0, p_reportObj->m_Buff, p_reportObj->m_End);
		LOG("Logged report: %s", p_reportObj->m_EventName);
	}

	GENERATE_CLASS_HOOK_NAMED(prepoSave, nn::prepo::PlayReport, Save) {
		logPrepo(p_this);
		return prepoSaveBak(p_this);
	}

	GENERATE_CLASS_HOOK_NAMED(prepoSaveWUid, nn::prepo::PlayReport, Save, nn::account::Uid const& uid) {
		logPrepo(p_this);
		return prepoSaveWUidBak(p_this, uid);
	}

	void prepoSaveHook(const std::string& gameStr) {
		if(gameStr.empty()) {
			LOG("gameStr can't be empty");
			return;
		}
		s_gameStr = gameStr;

		LOG("hooking prepoSave...");

		prepoSaveHook();
		prepoSaveWUidHook();
	}

} // namespace log
