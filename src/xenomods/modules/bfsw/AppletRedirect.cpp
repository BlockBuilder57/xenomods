#include "AppletRedirect.hpp"

#include "nn/account.h"
#include "nn/fs.h"
#include "xenomods/NnFile.hpp"
#include "xenomods/engine/applet/GameApplet.hpp"
#include "xenomods/engine/ml/Device.hpp"
#include "xenomods/engine/ml/Filesystem.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace {
	struct SelectMostRecentGame : skylaunch::hook::Trampoline<SelectMostRecentGame> {
		static void Hook() {
			if(xenomods::GetState().config.loadFcLatest && xenomods::AppletRedirect::IsMeliaRecentGame()) {
				applet::GameTitleMelia::execute();
				return;
			}
			Orig();
		}
	};
} // namespace

namespace xenomods {
	void AppletRedirect::Initialize() {
		SelectMostRecentGame::HookAt(&applet::GameTitle::execute);
	}

	bool AppletRedirect::IsMeliaRecentGame() {
		// We sort save files by last modified time to see if
		// Future Connected was the last game played. This matches
		// the behavior in XC3, where you need to have the latest
		// save in Future Redeemed to have it start by default.

		typedef nn::fs::DirectoryEntry DirEntry;

		nn::fs::DirectoryHandle dir {};
		if(R_FAILED(nn::fs::OpenDirectory(&dir, "save:/", nn::fs::OpenDirectoryMode::OpenDirectoryMode_File))) {
			xenomods::g_Logger->LogWarning("Could not open save dir");
			return false;
		}
		s64 entryCount = 0;
		s64 readCount = 0;
		// Keeps entry count at 0 on failure
		nn::fs::GetDirectoryEntryCount(&entryCount, dir);

		std::vector<DirEntry> entries(entryCount);
		if(R_FAILED(nn::fs::ReadDirectory(&readCount, entries.data(), dir, entryCount))) {
			entries.clear();
		} else {
			entries.resize(readCount);
		}
		nn::fs::CloseDirectory(dir);

		if (entries.empty())
			return false;

		std::vector<std::pair<std::string, unsigned long>> times;

		// Add the "save:/" prefix to all entries
		std::string prefix { "save:/" };
		for(auto& entry : entries) {
			auto name_str = std::string(entry.name);
			if(!name_str.ends_with(".sav") || !name_str.starts_with("bfs") || name_str.starts_with("bfssystem")) {
				continue;
			}

			ml::TimeInfo time;
			xenomods::NnFile file(prefix + name_str, nn::fs::OpenMode_Read);
			file.Seek(0x8, NnFile::SeekDirection::Beg); // offsetof(game::DataGameSave, timeInfo)
			file.ReadOne<ml::TimeInfo>(&time);
			times.push_back(std::pair(prefix + name_str, ml::TimeUtil::GetUnixTimestamp(time)));
			//g_Logger->LogInfo("ALL FILES {}: {}", name_str, ml::TimeUtil::GetFullInfo(time));
		}

		// sort them so the largest is at the end
		std::sort(times.begin(), times.end(), [=](std::pair<std::string, unsigned long>& a, std::pair<std::string, unsigned long>& b) {
			return a.second < b.second;
		});

		/*for(auto& entry : times) {
			g_Logger->LogInfo("SORTED {}: {}", entry.first, entry.second);
		}*/

		return std::string(times[times.size() - 1].first).starts_with("save:/bfsmeria");
	}

	XENOMODS_REGISTER_MODULE(AppletRedirect);
} // namespace xenomods
#endif
