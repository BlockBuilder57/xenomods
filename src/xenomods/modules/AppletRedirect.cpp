#include "AppletRedirect.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/Logger.hpp>

#include "xenomods/engine/applet/GameApplet.h"
#include "nn/fs.h"
#include "nn/account.h"
#include "xenomods/NnFile.hpp"
#include "xenomods/engine/ml/Filesystem.hpp"
#include "xenomods/engine/ml/Device.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace {
    struct SelectMostRecentGame : skylaunch::hook::Trampoline<SelectMostRecentGame> {
        static void Hook() {
            if (xenomods::AppletRedirect::Config.useRecentGame 
                    && xenomods::AppletRedirect::IsMeliaRecentGame()) {
                applet::GameTitleMelia::execute();
                return;
            }
            Orig();
        }
    };
}

namespace xenomods {
    AppletRedirect::ConfigState AppletRedirect::Config = {
        .useRecentGame = true
    };

    void AppletRedirect::Initialize() {
        SelectMostRecentGame::HookAt(&applet::GameTitle::execute);
    }

    bool AppletRedirect::IsMeliaRecentGame() {
        // We sort save files by last modified time to see if
        // Future Connected was the last game played. This matches
        // the behavior in XC3, where you need to have the latest
        // save in Future Redeemed to have it start by default.

        typedef nn::fs::DirectoryEntry DirEntry;

        nn::fs::DirectoryHandle dir{};
        if (R_FAILED(nn::fs::OpenDirectory(&dir, "save:/",
                        nn::fs::OpenDirectoryMode::OpenDirectoryMode_File))) {
            xenomods::g_Logger->LogWarning("Could not open save dir");
            return false;
        }
        s64 entryCount = 0;
        s64 readCount = 0;
        // Keeps entry count at 0 on failure
        nn::fs::GetDirectoryEntryCount(&entryCount, dir);

        std::vector<DirEntry> entries(entryCount);
        if (R_FAILED(nn::fs::ReadDirectory(&readCount, entries.data(), dir, entryCount))) {
            entries.clear();
        } else {
            entries.resize(readCount);
        }
        nn::fs::CloseDirectory(dir);

        // Add the "save:/" prefix to all entries
        std::string prefix{"save:/"};
        auto iter = entries.begin();
        while (iter != entries.end()) {
            auto& entry = *iter;
            auto name_str = std::string(entry.name);
            if (!name_str.ends_with(".sav") || name_str.starts_with("bfssystem")) {
                entries.erase(iter);
                continue;
            }
            std::rotate(std::begin(entry.name), std::end(entry.name) + prefix.size(), std::end(entry.name));
            std::copy(prefix.begin(), prefix.end(), std::begin(entry.name));
            ++iter;
        }

        auto mostRecent = std::max_element(entries.begin(), entries.end(), [](const DirEntry& e1, const DirEntry& e2) {
                return xenomods::NnFile::GetLastModified(e1.name) < xenomods::NnFile::GetLastModified(e2.name);
        });
        return mostRecent != entries.end() && std::string(mostRecent[0].name).starts_with("save:/bfsmeria");
    }

    XENOMODS_REGISTER_MODULE(AppletRedirect);
}
#endif
