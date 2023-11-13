// Created by block on 11/11/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "Data.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"
#include "xenomods/engine/mpgui/Mpgui.hpp"

#if XENOMODS_CODENAME(bfsw)

namespace fw {
	class Document;
}

namespace game {

	class Manager {
	   public:
		fw::Document* document;

		virtual void buildMpguiMessage(mpgui::MpguiContext* context);
		virtual mm::mtl::RTTI* getRTTI() const;
		virtual char* getName();
		virtual void initialize();
		virtual void finalize();
		virtual void update(const fw::UpdateInfo& updateInfo);
		virtual void postUpdate(const fw::UpdateInfo& updateInfo);
		virtual void postCameraUpdate(const fw::UpdateInfo& updateInfo);
		virtual void setInvalidTypeIndex();
	};

	class DataManager : public Manager {
	   public:
		INSERT_PADDING_BYTES(0x44);
		DataMenuSys dataMenuSys;
		INSERT_PADDING_BYTES(0x8);
		DataGame dataGame;

		// unfinished
		DataBdat dataBdat;
		DataParam dataParam;
		DataAI dataAI;
		DataSevFace dataSevFace;
		DataSpEff dataSpEff;
	};

} // namespace game
#endif