//
// Created by block on 7/28/2022.
//

#pragma once

#include <xenomods/Utils.hpp>

namespace event {
	enum MSG_TYPE : uint8_t {
		unk = 0,
		kDebug = 0x10,
		kAltDebug = 0x11
	};

	class Manager {
	   public:
		void update();
		void drawInfo();
		bool isPlayCancel() const;
	};

#define DEFINE_MANAGER(T)                   \
	struct T {                              \
		void setDisp(bool display);         \
		void OnEventProc(MSG_TYPE message); \
		void render();                      \
	}

	struct AgelogManager {
		void reserve(bool display);
		void setDisp(bool display) {
			reserve(display);
		}
		void OnEventProc(MSG_TYPE message);
		void render();
	};

	DEFINE_MANAGER(AlphaManager);
	DEFINE_MANAGER(BgmManager);

	struct BouManager {
		void setDebugEnable(bool display);
		void setDisp(bool display) {
			setDebugEnable(display);
		}
		void OnEventProc(MSG_TYPE message);
		void render();
	};

	DEFINE_MANAGER(CamManager);
	//DEFINE_MANAGER(CaptureManager); // has extra stuff, look into later
	DEFINE_MANAGER(CsSndManager);
	DEFINE_MANAGER(DebugManager);
	DEFINE_MANAGER(DofManager);
	DEFINE_MANAGER(EnvManager);

	struct EvtxtManager {
		void reserve(bool display);
		void setDisp(bool display) {
			reserve(display);
		}
		void OnEventProc(MSG_TYPE message);
		void render();
	};

	DEFINE_MANAGER(FrameManager);
	DEFINE_MANAGER(GroupManager);

	struct LightManager {
		void setLightDisp(bool display);
		void setDisp(bool display) {
			setLightDisp(display);
		}
		void OnEventProc(MSG_TYPE message);
		void render();
	};

	//DEFINE_MANAGER(ManiManager); // manipulator???
	DEFINE_MANAGER(MemoryManager);

	struct MovieManager {
		void setDisp(bool display) {}
		void OnEventProc(MSG_TYPE message);
		void render();
	};

	DEFINE_MANAGER(ObjectManager);
	DEFINE_MANAGER(OutsiderManager);
	DEFINE_MANAGER(ResManager);
	DEFINE_MANAGER(SeamlessManager);
	DEFINE_MANAGER(SeManager);
	DEFINE_MANAGER(SeqManager);
	DEFINE_MANAGER(TextManager);
	DEFINE_MANAGER(TodoManager);
	DEFINE_MANAGER(VoiceManager);
	DEFINE_MANAGER(VolumeManager);

#undef DEFINE_MANAGER

} // namespace event