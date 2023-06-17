#include "DebugStuff.hpp"

#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/fw/Debug.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/Debug.hpp"
#include "xenomods/engine/game/MenuGameData.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/game/SeqUtil.hpp"
#include "xenomods/engine/gf/BdatData.hpp"
#include "xenomods/engine/gf/Bgm.hpp"
#include "xenomods/engine/gf/Manager.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/gf/PlayFactory.hpp"
#include "xenomods/engine/gf/Weather.hpp"
#include "xenomods/engine/ml/DebugDrawing.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/ml/WinView.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/tl/title.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct MMAssert : skylaunch::hook::Trampoline<MMAssert> {
		static void Hook(const char* expr, const char* file, unsigned line) {
			xenomods::g_Logger->LogFatal("Caught Assert!!! Expr \"{}\" ({} : {})", expr, file, line);
			Orig(expr, file, line);
		}
	};

	struct BGMDebugging : skylaunch::hook::Trampoline<BGMDebugging> {
		static void Hook(gf::BgmTrack* this_pointer, fw::UpdateInfo* updateInfo) {
			Orig(this_pointer, updateInfo);

			if(!xenomods::DebugStuff::enableDebugRendering)
				return;

			const int height = xenomods::debug::drawFontGetHeight();
			if (this_pointer->getTrackName() != nullptr) {
				std::string trackName = this_pointer->getTrackName();

				if(trackName == "EventBGM")
					return; // already shown by event::BgmManager

				if(this_pointer->isPlaying()) {
					mm::mtl::FixStr<64> bgmFileName {};

					if(!this_pointer->makePlayFileName(bgmFileName)) {
						// failed to make a filename, just fall back to playingBgmFileName
						bgmFileName.set(this_pointer->playingBgmFileName);
					}

					xenomods::debug::drawFontFmtShadow(0, 720 - (xenomods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::white,
												 "{}: {} {:.1f}/{:.1f}{}", trackName, bgmFileName.buffer, this_pointer->getPlayTime(), this_pointer->getTotalTime(), this_pointer->isLoop() ? " (∞)" : "");
				} else {
					// uncomment if you want every BgmTrack instance to show
					//xenomods::debug::drawFontFmtShadow(0, 720 - (xenomods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::white, "{}: not playing", trackName);
				}
			}
		}
	};

#if XENOMODS_OLD_ENGINE
	struct JumpToClosedLandmarks_World : skylaunch::hook::Trampoline<JumpToClosedLandmarks_World> {
		static bool Hook(unsigned int mapjump) {
			return xenomods::DebugStuff::accessClosedLandmarks || Orig(mapjump);
		}
	};

	struct JumpToClosedLandmarks_Map : skylaunch::hook::Trampoline<JumpToClosedLandmarks_Map> {
		static bool Hook(unsigned int mapjump, mm::Pnt<short>* pos) {
			bool result = Orig(mapjump, pos);
			return xenomods::DebugStuff::accessClosedLandmarks || result;
		}
	};
#endif

#if XENOMODS_CODENAME(bfsw)
	struct EnableDebugUnlockAll : skylaunch::hook::Trampoline<EnableDebugUnlockAll> {
		static bool Hook(fw::Document* doc) {
			// the original always returns 0
			return xenomods::DebugStuff::enableDebugUnlockAll;
		}
	};
#endif
}

namespace xenomods {

	bool DebugStuff::enableDebugRendering = true;
	bool DebugStuff::enableDebugUnlockAll = false;
	bool DebugStuff::accessClosedLandmarks = false;

	int DebugStuff::tempInt = 0;
	int DebugStuff::bgmTrackIndex = 0;

	unsigned short DebugStuff::GetMapId() {
#if XENOMODS_OLD_ENGINE
		return gf::GfMapManager::getMapID();
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr != nullptr)
			return game::MenuGameDataMap::getPlayerStayingMapId(*DocumentPtr);
#endif
		return 0;
	}

	std::string DebugStuff::GetMapName(int id) {
		std::string value = "No Map";

		if(id > 0) {
#if XENOMODS_OLD_ENGINE
			value = gf::GfDataMap::getName(id);
#elif XENOMODS_CODENAME(bfsw)
			game::MenuGameDataMap dataMap(*DocumentPtr);
			dataMap.create(id, game::MenuGameDataMap::MapType::OneFloor);

			game::MsText text = dataMap.getMapNameText();

			if(text.pBdat != nullptr)
				value = Bdat::getMSText(text.pBdat, text.index);
			else
				value = "";
#endif
		}

		// display "ID n" if the name is blank
		if (value == "")
			value = "ID " + std::to_string(id);

		return value;
	}

	void DebugStuff::DoMapJump(int mapjumpId) {
		if (mapjumpId == 0)
			return;

#if !XENOMODS_CODENAME(bf3)
		int end = 1;
		unsigned char* pBdat =
#if XENOMODS_OLD_ENGINE
		Bdat::getFP("SYS_MapJumpList");
#elif XENOMODS_CODENAME(bfsw)
		Bdat::getFP("landmarklist");
#endif
		if (pBdat != nullptr)
			end = Bdat::getIdEnd(pBdat);

		// can input negative numbers to wrap to the end
		if (mapjumpId < 0)
			mapjumpId = end - (std::abs(mapjumpId) - 1);

		mapjumpId = std::clamp<unsigned>(mapjumpId, 1, end);
#endif

#if XENOMODS_OLD_ENGINE
		gf::GfPlayFactory::createSkipTravel(mapjumpId);
		gf::GfMenuObjUtil::playSE(gf::GfMenuObjUtil::SEIndex::mapjump);
#elif XENOMODS_CODENAME(bfsw)
		game::MapJumpSetupInfo info;

		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't do a map jump cause no doc ptr!");
			return;
		}

		//g_Logger->LogInfo("going to make info");
		game::SeqUtil::makeMapJumpSetupInfoFromLandmark(info, *DocumentPtr, mapjumpId);
		//g_Logger->LogInfo("made info, going to request jump");
		game::SeqUtil::requestMapJump(*DocumentPtr, info);
		//g_Logger->LogInfo("jump requested");
#endif
	}

	void MenuDoMapJump() {
		DebugStuff::DoMapJump(DebugStuff::tempInt);
	}

	void DebugStuff::PlaySE(unsigned int soundEffect) {
#if XENOMODS_OLD_ENGINE
		gf::GfMenuObjUtil::playSE(soundEffect);
#endif
	}

	void MenuPlaySE() {
		DebugStuff::PlaySE(DebugStuff::tempInt);
	}

	void DebugStuff::ReturnTitle(unsigned int slot) {
#if XENOMODS_OLD_ENGINE
		tl::TitleMain::returnTitle((gf::SAVESLOT)slot);
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't return to title cause no doc ptr!");
			return;
		}

		game::SeqUtil::returnTitle(*DocumentPtr);
#endif
	}

	void MenuReturnTitle() {
		DebugStuff::ReturnTitle(-1);
	}

	void DebugStuff::UpdateDebugRendering() {
#if XENOMODS_OLD_ENGINE
		fw::PadManager::enableDebugDraw(enableDebugRendering);
#endif
#if XENOMODS_CODENAME(bf3)
		unsigned int* globalDebugFlags = reinterpret_cast<unsigned int*>(skylaunch::utils::g_MainTextAddr + 0x1c49c60);

		// sets the system info print to display
		*globalDebugFlags ^= (-enableDebugRendering ^ *globalDebugFlags) & (1 << 6);
#endif
	}

	void DebugStuff::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up debug stuff...");

#if !XENOMODS_CODENAME(bf3)
		MMAssert::HookAt("_ZN2mm9MMStdBase8mmAssertEPKcS2_j");
#endif

#if XENOMODS_OLD_ENGINE
		BGMDebugging::HookAt("_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE");

		JumpToClosedLandmarks_World::HookAt(&gf::GfMenuObjWorldMap::isEnterMap);
		JumpToClosedLandmarks_Map::HookAt(&gf::GfMenuObjWorldMap::chkMapCond);
#elif XENOMODS_CODENAME(bfsw)
		EnableDebugUnlockAll::HookAt(&game::IsMenuDebugUnlockAll);
#endif

		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(DebugStuff), "Debug Stuff");

			section->RegisterOption<bool>(enableDebugRendering, "Enable debug rendering", &DebugStuff::UpdateDebugRendering);
#if XENOMODS_CODENAME(bfsw)
			section->RegisterOption<bool>(enableDebugUnlockAll, "Debug unlock all");
#endif
#if !XENOMODS_CODENAME(bf3)
	#if XENOMODS_OLD_ENGINE
			section->RegisterOption<bool>(accessClosedLandmarks, "Access closed landmarks");
	#endif
			section->RegisterOption<int>(tempInt, "Temp Int");
			section->RegisterOption<void>("Jump to Landmark", &MenuDoMapJump);
	#if XENOMODS_OLD_ENGINE
			section->RegisterOption<void>("Play common sound effect", &MenuPlaySE);
	#endif
			section->RegisterOption<void>("Return to Title", &MenuReturnTitle);
#endif
		}

		UpdateDebugRendering();
	}

	void DebugStuff::Update(fw::UpdateInfo* updateInfo) {
		bgmTrackIndex = 0;
	}

	XENOMODS_REGISTER_MODULE(DebugStuff);

} // namespace xenomods