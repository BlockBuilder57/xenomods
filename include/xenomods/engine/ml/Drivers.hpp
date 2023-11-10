// Created by block on 6/10/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/mm/FixStr.hpp"
#include "xenomods/engine/mm/mtl/PtrSingleton.hpp"

namespace ml {

	class Scn;
	class IDrDrawWorkInfo;

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
		INSERT_PADDING_BYTES(252);
		bool enableAA;
		float AASharpness;
		float AAEdgeThreshold;
		float AAEdgeThresholdMin;
		float AAEdgeRanges;
		bool unk272;
		bool enableTMAA;
		INSERT_PADDING_BYTES(2);
		INSERT_PADDING_BYTES(0x80);
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(8);
#endif
		bool enableSSAO;
		int blah;
		float SSAOBlurStrength;
		INSERT_PADDING_BYTES(384);
		float GBufferDebugParams[8][2];
		INSERT_PADDING_BYTES(0x128);
		bool GBufferDebug;
	};

	class DrResMdoTexList {
	   public:
		void texStmUpdate();
	};

	class DrCalcStmListObj {
	   public:
		INSERT_PADDING_BYTES(0x10);
		DrCalcStmListObj* nextListObj;
		INSERT_PADDING_BYTES(188);
		mm::mtl::FixStr<256> path;

		void chacheTexClear();
		void chacheDefClear();

		void update();

		void texstm_checkTexStm();
		void texstm_stmchk_topheader();
		void texstm_stmchk_topWait();
		void texstm_defMidTexUpdate();
		void texstm_defRead();
		void texstm_defupdate();
	};

	class DrCalcTexStreamMan : public mm::mtl::PtrSingleton<DrCalcTexStreamMan> {
	   public:
		INSERT_PADDING_BYTES(40);
		DrCalcStmListObj* rootObjectIDK;
		INSERT_PADDING_BYTES(40);

		// one for each buffer, oddly
		DrCalcStmListObj* updateList[512][2];
		int updateListCount[2];

		void update();
	};

}