// Created by block on 6/10/23.

#pragma once

#include <xenomods/Utils.hpp>

namespace ml {

	class Scn;

	class DrMan {
	   public:
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x3958);
#else
		INSERT_PADDING_BYTES(0x1D7C);
#endif
		bool hideChara; // 7548
		bool hideMap; // 7549

		INSERT_PADDING_BYTES(28);
		bool skipSomeRenderingThings;

		DrMan(Scn* scene);

		void GBuffRender();

		static DrMan* get();
	};

	class DrPixlPostParm {
	   public:
#if XENOMODS_OLD_ENGINE
		INSERT_PADDING_BYTES(0x320);
#else
		INSERT_PADDING_BYTES(0x328);
#endif
		float GBufferDebugParams[8][2];
		INSERT_PADDING_BYTES(0x128);
		bool GBufferDebug;
	};

}