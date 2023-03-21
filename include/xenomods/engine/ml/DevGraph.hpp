// Created by block on 3/20/23.

#pragma once

namespace ml {

	enum class DevGraphModeType {
		kUnknown
	};

	class DevGraph {
	   public:
		static void setVSync(int divisor);
		static int getVSync();

		static void setIgnore(bool ignore);
		static bool isIgnore();

		static void setRenderMode(DevGraphModeType mode);
		static DevGraphModeType getRenderMode();
	};

	namespace dg {
		static unsigned int s_nVSync;
		static unsigned int s_nVSyncOld;

		static bool s_bIgnore;
		static DevGraphModeType s_eGraphMode;
	} // namespace dg

} // namespace ml
