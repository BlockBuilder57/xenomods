#include "PlayerMovement.hpp"
#include "DebugStuff.hpp"

#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/Controllers.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/engine/gf/PlayerController.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

#if XENOMODS_OLD_ENGINE
	struct ApplyVelocityChanges : skylaunch::hook::Trampoline<ApplyVelocityChanges> {
		static void Hook(gf::GfComBehaviorPc* this_pointer, fw::UpdateInfo* updateInfo, gf::GfComPropertyPc* pcProperty) {
			//using enum gf::GfComPropertyPc::Flags;
			//bool flagInAir = (pcProperty->flags & static_cast<std::uint32_t>(InAir)) != 0;
			//bool flagOnWall = (pcProperty->flags & static_cast<std::uint32_t>(OnWall)) != 0;

			if(xenomods::PlayerMovement::moonJump)
				static_cast<glm::vec3&>(pcProperty->velocityActual).y = std::max(10.f, 10.f * (xenomods::PlayerMovement::movementSpeedMult / 64.f));

			auto& wish = static_cast<glm::vec3&>(pcProperty->velocityWish);

			wish *= xenomods::PlayerMovement::movementSpeedMult;

			//xenomods::debug::drawFontFmtShadow(0, 200, mm::Col4::white, "velAct {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityActual), pcProperty->velocityActual.XZLength());
			//xenomods::debug::drawFontFmtShadow(0, 216, mm::Col4::white, "velWsh {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityWish), pcProperty->velocityWish.XZLength());
			//xenomods::debug::drawFontFmtShadow(0, 232, mm::Col4::white, "who knows {:2} {:2}", static_cast<const glm::vec2&>(pcProperty->inputReal), static_cast<const glm::vec2&>(pcProperty->inputDupe));
			//xenomods::debug::drawFontFmtShadow(0, 248, mm::Col4::white, "flags: {:032b}", static_cast<std::uint32_t>(pcProperty->flags));
			//xenomods::debug::drawFontFmtShadow(0, 264, mm::Col4::white, "in air? {} on wall? {}", flagInAir, flagOnWall);

			Orig(this_pointer, updateInfo, pcProperty);

			wish /= xenomods::PlayerMovement::movementSpeedMult;
		}
	};

	struct DisableFallDamagePlugin : skylaunch::hook::Trampoline<DisableFallDamagePlugin> {
		static float Hook(gf::pc::FallDamagePlugin* this_pointer, mm::Vec3* vec) {
			return xenomods::PlayerMovement::disableFallDamage ? 0.f : Orig(this_pointer, vec);
		}
	};

	struct DisableStateUtilFallDamage : skylaunch::hook::Trampoline<DisableStateUtilFallDamage> {
		static void Hook(gf::GfComBehaviorPc* GfComBehaviorPc, bool param_2) {
			//xenomods::g_Logger->LogInfo("StateUtil::setFallDamageDisable(GfComBehaviorPc: {:p}, bool: {})", GfComBehaviorPc, param_2);
			Orig(GfComBehaviorPc, xenomods::PlayerMovement::disableFallDamage || param_2);
		}
	};

	struct CorrectCameraTarget : skylaunch::hook::Trampoline<CorrectCameraTarget> {
		static void Hook(gf::PlayerCameraTarget* this_pointer) {
			Orig(this_pointer);
			if(xenomods::PlayerMovement::moonJump) {
				// makes the game always take the on ground path in gf::PlayerCamera::updateTracking
				this_pointer->inAir = false;
				// should stop the camera from suddenly jerking back to the maximum height moonjumped to
				this_pointer->surfaceHeight = static_cast<glm::vec3&>(this_pointer->moverPos).y;
				this_pointer->aboveWalkableGround = true;
			}
		}
	};

#elif XENOMODS_NEW_ENGINE

	bool ValidToChange(game::CharacterController* cc) {
		if(cc->actorAccessor != nullptr && cc->actorAccessor->isValid()) {
			auto behavior = cc->actorAccessor->getBehaviorComponent();
			if(behavior->getRTTI()->isKindOf(&game::BehaviorPc::m_rtti))
				return true;
		}
		return false;
	}

	struct ApplyVelocityChanges : skylaunch::hook::Trampoline<ApplyVelocityChanges> {
		static void Hook(game::CharacterController* this_pointer, mm::Vec3& pos, bool param_2) {
			if(ValidToChange(this_pointer)) {
				auto& impulse = static_cast<glm::vec3&>(pos);
				auto& velocity = static_cast<glm::vec3&>(this_pointer->velocity);

				impulse *= xenomods::PlayerMovement::movementSpeedMult;

				// undo the y changes when we're not on a wall
				if(!xenomods::bitMask(this_pointer->flags, game::CharacterController::Flags::pcClimb))
					impulse.y /= xenomods::PlayerMovement::movementSpeedMult;

				if(xenomods::PlayerMovement::moonJump) {
					velocity.y = 0;
					impulse.y = std::max(0.5f, 0.5f * (xenomods::PlayerMovement::movementSpeedMult / 64.f));
				}
			}

			Orig(this_pointer, pos, param_2);
		}
	};

	struct DisableFallDamage : skylaunch::hook::Trampoline<DisableFallDamage> {
		static float Hook(game::CharacterController* this_pointer) {
			return ValidToChange(this_pointer) && xenomods::PlayerMovement::disableFallDamage ? 0 : Orig(this_pointer);
		}
	};

#endif

} // namespace

namespace xenomods {

	bool PlayerMovement::moonJump = false;
	bool PlayerMovement::disableFallDamage = false;
	float PlayerMovement::movementSpeedMult = 1.f;

	std::vector<PlayerMovement::WarpData> PlayerMovement::Warps = {};

	bool PlayerMovement::ShowWarpsWindow = false;
	bool PlayerMovement::ShowAllWarps = false;
	bool PlayerMovement::ShowWarpsOnMap = false;

	glm::vec3* PlayerMovement::GetPartyPosition() {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			return reinterpret_cast<glm::vec3*>(trans->getPosition());
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				return reinterpret_cast<glm::vec3*>(&control->position);
			}
		}
#endif
		return nullptr;
	}
	void PlayerMovement::SetPartyPosition(glm::vec3 pos) {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			trans->setPosition(pos);
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				control->syncFrame();
				control->setWarp(pos, 5);
				control->clearLanding();
			}
		}
#endif
	}
	glm::quat* PlayerMovement::GetPartyRotation() {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			return reinterpret_cast<glm::quat*>(trans->getRotation());
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				return reinterpret_cast<glm::quat*>(&control->rotation);
			}
		}
#endif
		return nullptr;
	}
	void PlayerMovement::SetPartyRotation(glm::quat rot) {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			trans->setRotation(rot);
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				control->setQuat(rot);
			}
		}
#endif
	}

	glm::vec3* PlayerMovement::GetPartyVelocity() {
#if XENOMODS_OLD_ENGINE
		// TODO
		/*gf::GF_OBJ_HANDLE handle = gf::GfGameParty::getLeader();
		if (handle.IsValid()) {
			g_Logger->LogInfo("Got handle - {}", reinterpret_cast<unsigned long>(handle.Ptr()));

			auto acc = gf::GfObjAcc(handle.Ptr());
			g_Logger->LogInfo("Supposed type: {}", (int)acc.getType());

			void* thing = gf::GfObjUtil::getProperty(handle.Ptr());
			g_Logger->LogInfo("What's the thing? {}", thing);
			auto* behavior = reinterpret_cast<gf::GfComPropertyPc*>(thing);
			if (behavior != nullptr) {
				g_Logger->LogInfo("where is its rtti? {}", reinterpret_cast<void*>(behavior->getRTTI()));
				if (behavior->getRTTI() != nullptr) {
					g_Logger->LogInfo("rtti says: {}", behavior->getRTTI()->szName);
					if (behavior->getRTTI()->isKindOf(&gf::GfComPropertyPc::m_rtti)) {
						g_Logger->LogInfo("Wario");
					}
				}
				else
					g_Logger->LogInfo("uh oh, no RTTI");
			}
		}*/
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				return reinterpret_cast<glm::vec3*>(&control->velocity);
			}
		}
#endif
		return nullptr;
	}
	void PlayerMovement::SetPartyVelocity(glm::vec3 vel) {
#if XENOMODS_OLD_ENGINE
		// TODO
#elif XENOMODS_CODENAME(bfsw)
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if(leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if(control != nullptr) {
				control->clearVelocity();
				control->addLinearVelocity(vel);
			}
		}
#endif
	}

	void PlayerMovement::LoadWarpsFromFile() {
		auto path = fmt::format(XENOMODS_CONFIG_PATH "/{}/warps.toml", XENOMODS_CODENAME_STR);
		toml::parse_result res = toml::parse_file(path);

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogDebug("Warp file error: ({})", error.description());
			return;
		}

		auto table = std::move(res).table();

		if(table["warps"].type() != toml::node_type::array)
			return;

		auto arr = table.get_as<toml::array>("warps");

		if(arr->size() > 0) {
			Warps.clear();

			arr->for_each([&](auto& el) {
				WarpData warp {};

				toml::table& thisone = *el.as_table();

				if(thisone["name"].type() == toml::node_type::string)
					warp.name = thisone["name"].value_or<std::string>("no name");

				if(thisone["mapId"].type() == toml::node_type::integer) {
					warp.mapId = thisone["mapId"].value_or(0);
					if(detail::IsModuleRegistered(STRINGIFY(DebugStuff)))
						warp.mapName = DebugStuff::GetMapName(warp.mapId);
					else
						warp.mapName = "bepis";
				}

				if(thisone["position"].type() == toml::node_type::array) {
					warp.position.x = thisone["position"][0].value_or(0.f);
					warp.position.y = thisone["position"][1].value_or(0.f);
					warp.position.z = thisone["position"][2].value_or(0.f);
				}

				if(thisone["rotation"].type() == toml::node_type::array) {
					int size = thisone["rotation"].as_array()->size();

					if(size == 3) {
						glm::vec3 temp;

						temp.x = thisone["rotation"][0].value_or(0.f);
						temp.y = thisone["rotation"][1].value_or(0.f);
						temp.z = thisone["rotation"][2].value_or(0.f);

						warp.rotation = glm::quat(glm::radians(temp));
					} else if(size == 4) {
						warp.rotation.x = thisone["rotation"][0].value_or(1.f);
						warp.rotation.y = thisone["rotation"][1].value_or(0.f);
						warp.rotation.z = thisone["rotation"][2].value_or(0.f);
						warp.rotation.w = thisone["rotation"][2].value_or(0.f);
					}

					warp.rotationEuler = glm::degrees(glm::eulerAngles(warp.rotation));
				}

				if(thisone["velocity"].type() == toml::node_type::array) {
					warp.velocity.x = thisone["velocity"][0].value_or(0.f);
					warp.velocity.y = thisone["velocity"][1].value_or(0.f);
					warp.velocity.z = thisone["velocity"][2].value_or(0.f);
				}

				Warps.push_back(warp);
			});

			g_Logger->ToastInfo("warp", "Loaded {} warp(s)", Warps.size());
		}
	}

	void PlayerMovement::SaveWarpsToFile() {
		auto path = fmt::format(XENOMODS_CONFIG_PATH "/{}/warps.toml", XENOMODS_CODENAME_STR);

		toml::array allWarps;

		for(auto& warp : Warps) {
			toml::table thisone;

			thisone.emplace("name", std::string(warp.name));
			thisone.emplace("mapId", warp.mapId);
			// save the name of the map to the file so it's clearer what map each warp is for
			thisone.emplace("mapNameReadOnly", warp.mapName);

			thisone.emplace("position", toml::array { warp.position.x, warp.position.y, warp.position.z });
			thisone.emplace("rotation", toml::array { warp.rotationEuler.x, warp.rotationEuler.y, warp.rotationEuler.z });
			thisone.emplace("velocity", toml::array { warp.velocity.x, warp.velocity.y, warp.velocity.z });

			allWarps.emplace_back(thisone);
		}

		toml::table finalTable;
		finalTable.emplace("warps", allWarps);

		std::stringstream ss;
		ss << finalTable;
		std::string out = ss.str();

		if(!NnFile::Preallocate(path, out.size())) {
			g_Logger->LogError("Couldn't create/preallocate warps file \"{}\"", path);
			return;
		}

		NnFile file(path, nn::fs::OpenMode_Write);

		file.Write(out.c_str(), out.size());
		file.Flush();
		file.Close();
	}

	PlayerMovement::WarpData* PlayerMovement::NewWarp() {
		unsigned short mapId = 0;
		std::string mapName = "bepis";

		if(detail::IsModuleRegistered(STRINGIFY(DebugStuff))) {
			mapId = DebugStuff::GetMapId();
			mapName = DebugStuff::GetMapName();
		}

		WarpData warp = {
			.name = mapName + " Warp " + std::to_string(Warps.size()),
			.mapName = mapName,
			.mapId = mapId,
		};
		Warps.push_back(warp);

		// there's probably a better way of doing this...
		WarpData* pWarp = &Warps[Warps.size() - 1];
		SetWarp(pWarp);
		return pWarp;
	}
	void PlayerMovement::SetWarp(WarpData* warp) {
		if(xenomods::detail::IsModuleRegistered(STRINGIFY(DebugStuff))) {
			warp->mapId = DebugStuff::GetMapId();
			warp->mapName = DebugStuff::GetMapName(warp->mapId);
		} else {
			warp->mapId = 0;
			warp->mapName = "bepis";
		}

		if(GetPartyPosition() != nullptr)
			warp->position = *GetPartyPosition();
		if(GetPartyRotation() != nullptr)
			warp->rotation = *GetPartyRotation();
		if(GetPartyVelocity() != nullptr)
			warp->velocity = *GetPartyVelocity();

		warp->rotationEuler = glm::degrees(glm::eulerAngles(warp->rotation));

		g_Logger->ToastInfo("warp", "Set {} at {:3}", warp->name, warp->position);
		g_Logger->ToastInfo("warp2", "(rot {} vel {})", warp->rotation, warp->velocity);
	}
	void PlayerMovement::DeleteWarp(WarpData* warp) {
		for(auto iter = Warps.begin(); iter != Warps.end(); iter++) {
			if(iter.base() == warp) {
				Warps.erase(iter);
				break;
			}
		}
	}

	void PlayerMovement::GoToWarp(WarpData* warp) {
		// lock out warps that have map ids set
		// if DebugStuff isn't present just let them do whatever
		if(xenomods::detail::IsModuleRegistered(STRINGIFY(DebugStuff))) {
			if(warp->mapId > 0 && warp->mapId != DebugStuff::GetMapId()) {
				g_Logger->ToastWarning("warp", "Current map ({}) does not match the warp's map ({})", DebugStuff::GetMapName(), DebugStuff::GetMapName(warp->mapId));
				return;
			}
		}

		const glm::quat zero = glm::quat(0, 0, 0, 0);
		glm::quat normalized = glm::normalize(warp->rotation);

		SetPartyPosition(warp->position);
		if(normalized != zero)
			SetPartyRotation(normalized); // can cause fun errors
		SetPartyVelocity(warp->velocity);

		g_Logger->ToastInfo("warp", "Warped party to {}", warp->name);
	}

	std::string cacheMapName = "No Map";

	std::string MenuCurrentMapInfo() {
		return cacheMapName;
	}

	std::string MenuCurrentPlayerPosition() {
		auto pos = xenomods::PlayerMovement::GetPartyPosition();
		if(pos != nullptr)
			return fmt::format("{:2}", *pos);
		else
			return "-";
	}
	std::string MenuCurrentPlayerRotation() {
		auto rot = xenomods::PlayerMovement::GetPartyRotation();
		if(rot != nullptr)
			return fmt::format("{:1}", glm::degrees(glm::eulerAngles(*rot)));
		else
			return "-";
	}
	std::string MenuCurrentPlayerVelocity() {
		auto vel = xenomods::PlayerMovement::GetPartyVelocity();
		if(vel != nullptr)
			return fmt::format("{:2}", *vel);
		else
			return "-";
	}

	void PlayerMovement::MenuSection() {
		ImGui::MenuItem("Show Warps Window", "", &ShowWarpsWindow);
		ImGui::Checkbox("Disable fall damage", &disableFallDamage);
		ImGui::PushItemWidth(150.f);
		imguiext::InputFloatExt("Movement speed multiplier", &movementSpeedMult, 1.f, 5.f, 2.f, "%.2f");
		ImGui::PopItemWidth();
	}

	void PlayerMovement::MenuState() {
		ImGui::Text("Current map: %s", MenuCurrentMapInfo().c_str());
		ImGui::Text("Current pos: %s", MenuCurrentPlayerPosition().c_str());
		ImGui::Text("Current rot: %s", MenuCurrentPlayerRotation().c_str());
		ImGui::Text("Current vel: %s", MenuCurrentPlayerVelocity().c_str());
	}

	void MenuDrawWarp(PlayerMovement::WarpData* warp) {
		ImGui::Text("Map: %s", warp->mapName.c_str());
		ImGui::DragFloat3("Position", reinterpret_cast<float*>(&warp->position));
		if(ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&warp->rotationEuler)))
			warp->rotation = glm::quat(glm::radians(warp->rotationEuler));
		ImGui::DragFloat3("Velocity", reinterpret_cast<float*>(&warp->velocity));

		if(ImGui::Button("Go To Warp"))
			PlayerMovement::GoToWarp(warp);
		ImGui::SameLine();
		if(ImGui::Button("Overwrite Warp"))
			PlayerMovement::SetWarp(warp);
		ImGui::SameLine();
		if(ImGui::Button("Delete Warp"))
			PlayerMovement::DeleteWarp(warp);
	}

	void PlayerMovement::MenuWarps() {
		if(!ShowWarpsWindow)
			return;

		if(!ImGui::Begin("Warps", &ShowWarpsWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::End();
			return;
		}

		if(ImGui::Button("Load Warps From File"))
			LoadWarpsFromFile();
		ImGui::SameLine();
		if(ImGui::Button("Save Warps To File"))
			SaveWarpsToFile();

		ImGui::Checkbox("Show all warps", &ShowAllWarps);
		ImGui::SameLine();
		ImGui::Checkbox("Show warps on map", &ShowWarpsOnMap);

		if(ImGui::BeginTabBar("WarpsBar", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyScroll)) {
			// new warp button
			if(ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
				NewWarp();

			// draw a tab for each warp
			for(int n = 0; n < Warps.size();) {
				WarpData* warp = &Warps[n];
				bool open = true;

				// disabling show all warps will only show the warps that are active on the current map
				if(!ShowAllWarps && detail::IsModuleRegistered(STRINGIFY(DebugStuff))) {
					if(warp->mapId > 0 && warp->mapId != DebugStuff::GetMapId()) {
						n++;
						continue;
					}
				}

				if(ImGui::BeginTabItem(warp->name.c_str(), &open, ImGuiTabItemFlags_None)) {
					MenuDrawWarp(warp);
					ImGui::EndTabItem();
				}

				if(!open)
					DeleteWarp(warp);
				else
					n++;
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	void PlayerMovement::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up player movement hooks...");

#if XENOMODS_OLD_ENGINE
		ApplyVelocityChanges::HookAt("_ZN2gf15GfComBehaviorPc19integrateMoveNormalERKN2fw10UpdateInfoERNS_15GfComPropertyPcE");

		DisableFallDamagePlugin::HookAt("_ZNK2gf2pc18FallDistancePlugin12calcDistanceERKN2mm4Vec3E");
		DisableStateUtilFallDamage::HookAt("_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb");

		CorrectCameraTarget::HookAt("_ZN2gf18PlayerCameraTarget15writeTargetInfoEv");
#elif XENOMODS_CODENAME(bfsw)
		ApplyVelocityChanges::HookAt(&game::CharacterController::applyMoveVec);
		DisableFallDamage::HookAt(&game::CharacterController::getFallHeight);
		// TODO: look into game::PcStateUtil::updateClimbMove for vertical climbing speed
#endif

#if !XENOMODS_CODENAME(bf3) // need to find these for 3
		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(PlayerMovement), "Player Movement");
			section->RegisterRenderCallback(&MenuSection);
		}

		auto state = g_Menu->FindSection("state");
		if(state != nullptr) {
			state->RegisterRenderCallback(&MenuState);
		}

		g_Menu->RegisterRenderCallback(&MenuWarps);
#endif
	}

	void PlayerMovement::Update(fw::UpdateInfo* updateInfo) {
		moonJump = HidInput::GetPlayer(1)->InputHeld(Keybind::MOONJUMP);

#if !XENOMODS_CODENAME(bf3)
		if(ShowWarpsOnMap) {
			if(!detail::IsModuleRegistered(STRINGIFY(DebugStuff)))
				return;

			for(WarpData warp : Warps) {
				// skip warps that won't work
				if(warp.mapId > 0 && warp.mapId != DebugStuff::GetMapId())
					continue;

				// axis/arrow rendering is messed up in DE for some reason, it's all relative to the camera center...
	#if XENOMODS_OLD_ENGINE
				float angle = glm::angle(warp.rotation);
				glm::vec3 axis = glm::axis(warp.rotation);

				glm::mat4 newmat = glm::mat4(1.f);
				newmat = glm::translate(newmat, warp.position);
				newmat = glm::rotate(newmat, angle, axis);

				fw::debug::drawCompareZ(false);
				fw::debug::drawAxis(newmat, 2.f);
				fw::debug::drawArrow(warp.position, warp.position + (warp.velocity * 0.5f), mm::Col4::yellow);
				fw::debug::drawCompareZ(true);
	#endif

				xenomods::debug::drawFontFmtShadow3D(warp.position, mm::Col4::white, "{}", warp.name);
			}
		}
#endif
	}

	void PlayerMovement::OnMapChange(unsigned short mapId) {
		if(detail::IsModuleRegistered(STRINGIFY(DebugStuff)))
			cacheMapName = DebugStuff::GetMapName(mapId);

		if(!cacheMapName.starts_with("ID"))
			cacheMapName += " (ID " + std::to_string(mapId) + ")";
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(PlayerMovement);
#endif

} // namespace xenomods
