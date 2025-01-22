#include "DebugStuff.hpp"

#include "xenomods/engine/apps/FrameworkLauncher.hpp"
#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/fw/Debug.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/Debug.hpp"
#include "xenomods/engine/game/MenuGameData.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/gf/Bgm.hpp"
#include "xenomods/engine/gf/Data.hpp"
#include "xenomods/engine/gf/Manager.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/gf/PlayFactory.hpp"
#include "xenomods/engine/gf/Weather.hpp"
#include "xenomods/engine/gmk/Landmark.hpp"
#include "xenomods/engine/ml/DebugDrawing.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/ml/WinView.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/mm/StdBase.hpp"
#include "xenomods/engine/mtl/Allocator.hpp"
#include "xenomods/engine/mtl/MemManager.hpp"
#include "xenomods/engine/mtl/MemoryInfo.hpp"
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

#if XENOMODS_OLD_ENGINE
	struct BGMDebugging : skylaunch::hook::Trampoline<BGMDebugging> {
		static void Hook(gf::BgmTrack* this_pointer, fw::UpdateInfo* updateInfo) {
			Orig(this_pointer, updateInfo);

			if(!xenomods::DebugStuff::enableDebugRendering)
				return;

			const int height = xenomods::debug::drawFontGetHeight();
			if(this_pointer->getTrackName() != nullptr) {
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

	struct JumpToClosedLandmarks_CanEnterMap : skylaunch::hook::Trampoline<JumpToClosedLandmarks_CanEnterMap> {
		static bool Hook(unsigned int mapjump) {
			return xenomods::DebugStuff::accessClosedLandmarks || Orig(mapjump);
		}
	};

	struct JumpToClosedLandmarks_CheckCondition : skylaunch::hook::Trampoline<JumpToClosedLandmarks_CheckCondition> {
		static bool Hook(unsigned int mapjump, mm::Pnt<short>* pos) {
			bool result = Orig(mapjump, pos);
			return xenomods::DebugStuff::accessClosedLandmarks || result;
		}
	};

	struct JumpToClosedLandmarks_WorldMap : skylaunch::hook::Trampoline<JumpToClosedLandmarks_WorldMap> {
		static bool Hook(const gf::MenuZoneMapInfo& info) {
			bool result = Orig(info);
			return xenomods::DebugStuff::accessClosedLandmarks || result;
		}
	};
	struct JumpToClosedLandmarks_ZoneMap : skylaunch::hook::Trampoline<JumpToClosedLandmarks_ZoneMap> {
		static bool Hook(const gf::MenuZoneMapInfo& info) {
			bool result = Orig(info);
			return xenomods::DebugStuff::accessClosedLandmarks || result;
		}
	};

	struct JumpToClosedLandmarks_IsFound : skylaunch::hook::Trampoline<JumpToClosedLandmarks_IsFound> {
		static bool Hook(gmk::GmkLandmark* this_pointer) {
			bool result = Orig(this_pointer);
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

	struct AlwaysAbleToOpenMenu : skylaunch::hook::Trampoline<AlwaysAbleToOpenMenu> {
		static bool Hook(fw::Document* doc) {
			return !xenomods::DebugStuff::enableDebugUnlockAll && Orig(doc);
		}
	};
#endif
} // namespace

namespace xenomods {

	bool DebugStuff::enableDebugRendering = false;
	bool DebugStuff::enableDebugUnlockAll = false;
	bool DebugStuff::accessClosedLandmarks = false;
	bool DebugStuff::pauseEnable = false;
	bool DebugStuff::enableMemoryDebug = false;

	std::int8_t DebugStuff::pauseStepForward = 0;
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
		if(value == "")
			value = "ID " + std::to_string(id);

		return value;
	}

	void DebugStuff::DoMapJump(int mapjumpId) {
		if(mapjumpId == 0)
			return;

#if !XENOMODS_CODENAME(bf3)
		int end = 1;
		unsigned char* pBdat =
	#if XENOMODS_OLD_ENGINE
		Bdat::getFP("SYS_MapJumpList");
	#elif XENOMODS_CODENAME(bfsw)
		Bdat::getFP("landmarklist");
	#else
		"invalid";
	#endif
		if(pBdat != nullptr)
			end = Bdat::getIdEnd(pBdat);

		// can input negative numbers to wrap to the end
		if(mapjumpId < 0)
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

	void DebugStuff::PlaySE(unsigned int soundEffect) {
#if XENOMODS_OLD_ENGINE
		gf::GfMenuObjUtil::playSE(soundEffect);
#endif
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

	void DebugStuff::UpdateDebugRendering() {
#if XENOMODS_OLD_ENGINE
		fw::PadManager::enableDebugDraw(enableDebugRendering);
#endif
#if XENOMODS_CODENAME(bf3)
		unsigned int* s_flg = nullptr; //ml::_dsk::s_flg

		if(version::RuntimeVersion() == version::SemVer::v2_0_0)
			s_flg = reinterpret_cast<unsigned int*>(skylaunch::utils::AddrFromBase(0x7101c49c60));
		else if(version::RuntimeVersion() == version::SemVer::v2_1_0 || version::RuntimeVersion() == version::SemVer::v2_1_1 || version::RuntimeVersion() == version::SemVer::v2_2_0)
			s_flg = reinterpret_cast<unsigned int*>(skylaunch::utils::AddrFromBase(0x7101c4ac60));

		// sets the system info print to display
		if(s_flg != nullptr)
			*s_flg ^= (-enableDebugRendering ^ *s_flg) & (1 << 6);
#endif
	}

	void DebugStuff::MemoryDebugRendering() {
		if(!DebugStuff::enableMemoryDebug)
			return;

		static std::array<std::pair<int, mtl::MemoryInfo>, 511> memInfos;
		static int lastActiveRegions;

		mtl::AllocHandle allocHandle { 0 };
		bool open = true;

		if(!ImGui::Begin("Memory", &open)) {
			ImGui::End();
			return;
		}

		size_t activeRegions = 0;
		for(int i = 0; i < memInfos.max_size(); i++) {
			allocHandle.regionId = i + 1;
			if(!mtl::MemManager::GET_MEMORY_INFO(&allocHandle, &memInfos[i].second)) {
				break;
			}
			memInfos[i].first = i + 1;
			activeRegions++;
		}

		if(ImGui::BeginTable("memdbg", 5, ImGuiTableFlags_Sortable)) {
			// Headers
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 20.0, 1);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0.0, 2);
			ImGui::TableSetupColumn("Used %", ImGuiTableColumnFlags_None, 0.0, 3);
			ImGui::TableSetupColumn("Allocated (MB)", ImGuiTableColumnFlags_None, 0.0, 4);
			ImGui::TableSetupColumn("Total (MB)", ImGuiTableColumnFlags_None, 0.0, 5);
			ImGui::TableHeadersRow();

			auto sortSpecs = ImGui::TableGetSortSpecs();
			// Only sort when a sort key is chosen, and only sort when necessary
			if(sortSpecs != nullptr && (lastActiveRegions != activeRegions || sortSpecs->SpecsDirty || sortSpecs->Specs[0].ColumnUserID >= 2)) {
				std::sort(memInfos.begin(), memInfos.begin() + activeRegions, [&sortSpecs](const auto& a, const auto& b) -> bool {
					bool cmp;
					switch(sortSpecs->Specs[0].ColumnUserID) {
						case 1:
							cmp = a.first < b.first;
							break;
						case 2:
							cmp = std::strcmp(a.second.regionName, b.second.regionName) < 0;
							break;
						case 3:
							cmp = a.second.usedPercent < b.second.usedPercent;
							break;
						case 4:
							cmp = a.second.allocatedSize < b.second.allocatedSize;
							break;
						case 5:
							cmp = a.second.totalSize < b.second.totalSize;
							break;
						default:
							IM_ASSERT(0);
							break;
					}
					return sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending ? cmp : !cmp;
				});
			}

			for(int i = 0; i < activeRegions; i++) {
				auto memInfo = &memInfos[i].second;
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Text("%d", memInfos[i].first);

				ImGui::TableNextColumn();
				ImGui::Text(memInfo->regionName);

				// Used % progress bar
				ImU32 color;
				if(memInfo->usedPercent >= 90)
					color = IM_COL32(161, 21, 13, 255);
				else if(memInfo->usedPercent >= 50)
					color = IM_COL32(163, 108, 13, 255);
				else
					color = IM_COL32(28, 82, 52, 255);
				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
				ImGui::ProgressBar(memInfo->usedPercent / 100, ImVec2(ImGui::GetFontSize() * 10, 0.0f), std::format("{:.1f}%", memInfo->usedPercent).c_str());
				ImGui::PopStyleColor(1);

				ImGui::TableNextColumn();
				ImGui::Text("%.4f", memInfo->allocatedSize / 1e6);
				ImGui::TableNextColumn();
				ImGui::Text("%.4f", memInfo->totalSize / 1e6);
			}
			ImGui::EndTable();
		}
		lastActiveRegions = activeRegions;

		ImGui::End();
	}

	void DebugStuff::MenuSection() {
		if(ImGui::Checkbox("Enable debug rendering", &DebugStuff::enableDebugRendering))
			DebugStuff::UpdateDebugRendering();
		ImGui::Checkbox("Memory debug window", &DebugStuff::enableMemoryDebug);

		/*ImGui::Checkbox("Pause updates", &DebugStuff::pauseEnable);
		ImGui::SameLine();
		if (ImGui::Button("Step Frame"))
			pauseStepForward = 1;
		ImGui::SameLine();
		if (ImGui::Button("Step Sec"))
			pauseStepForward = 30;*/

#if XENOMODS_CODENAME(bfsw)
		ImGui::Checkbox("Debug unlock all", &DebugStuff::enableDebugUnlockAll);
#endif
#if !XENOMODS_CODENAME(bf3)
	#if XENOMODS_OLD_ENGINE
		ImGui::Checkbox("Access closed landmarks", &DebugStuff::accessClosedLandmarks);
	#endif
		ImGui::PushItemWidth(150.f);
		ImGui::InputInt("Temp Int", &DebugStuff::tempInt);
		ImGui::PopItemWidth();
		if(ImGui::Button("Jump to Landmark"))
			DebugStuff::DoMapJump(DebugStuff::tempInt);
	#if XENOMODS_OLD_ENGINE
		ImGui::SameLine();
		if(ImGui::Button("Play common sound effect"))
			DebugStuff::PlaySE(DebugStuff::tempInt);
	#endif
		if(ImGui::Button("Return to Title"))
			DebugStuff::ReturnTitle();
#endif
	}

	void DebugStuff::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up debug stuff...");

#if !XENOMODS_CODENAME(bf3)
		MMAssert::HookAt(&mm::MMStdBase::mmAssert);
#endif

#if XENOMODS_OLD_ENGINE
		BGMDebugging::HookAt("_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE");

		JumpToClosedLandmarks_CanEnterMap::HookAt(&gf::GfMenuObjWorldMap::isEnterMap);
		JumpToClosedLandmarks_CheckCondition::HookAt(&gf::GfMenuObjWorldMap::chkMapCond);
		JumpToClosedLandmarks_WorldMap::HookAt(&gf::GfMenuObjWorldMap::isEnableJump);
		JumpToClosedLandmarks_ZoneMap::HookAt(&gf::GfMenuObjZoneMap::isEnableJump);
		JumpToClosedLandmarks_IsFound::HookAt("_ZNK3gmk11GmkLandmark7isFoundEv");
#elif XENOMODS_CODENAME(bfsw)
		EnableDebugUnlockAll::HookAt(&game::IsMenuDebugUnlockAll);
		AlwaysAbleToOpenMenu::HookAt(&game::DataUtil::isDisableMenu);
#endif

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(DebugStuff), "Debug Stuff");
			section->RegisterRenderCallback(&MenuSection);
		}

		UpdateDebugRendering();

		xenomods::g_Menu->RegisterRenderCallback(&DebugStuff::MemoryDebugRendering, false);
	}

	void DebugStuff::Update(fw::UpdateInfo* updateInfo) {
		bgmTrackIndex = 0;

		if(pauseEnable && pauseStepForward > 0) {
			pauseStepForward--;
		}
	}

	XENOMODS_REGISTER_MODULE(DebugStuff);

} // namespace xenomods