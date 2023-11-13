// Created by block on 3/20/23.

#pragma once

#include "xenomods/engine/mm/mtl/FixStr.hpp"

namespace ml {

	class ProcDesktop {
	   public:
		static void setBuildRevision(const char* rev);

		// Seems to be inconsistent with the format?
		// 2 2.0.0     - Rev.295054
		// Torna 1.0.0 - Jul 18 2018 20:09:03
		// DE 1.1.2    - Rev.68319 ms.6035
		static mm::mtl::FixStr<128>* getBuildRevision();
	};

}