// Created by block on 3/21/23.

#pragma once

namespace gf {

	typedef int CLOUD_SEA_HANDLE;

	enum class CLOUD_SEA {
		OFFMAYBE = 0,
		Type1,
		Type2,
		Type3,
		Type4,
		Type5
	};

	enum class CLOUD_SEA_PRIO {
		HIGH,
		LOW
	};

	class GfGameEnv {
	   public:
		static void createCloudSea(CLOUD_SEA param_1, CLOUD_SEA_PRIO param_2);
	};

	class GfFieldWeather {
	   public:
		static void setCloudSeaForDebug(unsigned int param_1, CLOUD_SEA param_2);
		static void clear();
	};

}