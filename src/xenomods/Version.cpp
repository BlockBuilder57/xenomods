// Created by block on 2/10/2023.

#include "Version.hpp"

#include "nn/oe.h"
#include "skylaunch/nx/kernel/svc.h"

namespace xenomods::version {

	unsigned long RuntimeProgramID() {
		static unsigned long progid = 0;

		if (progid != 0)
			return progid;

		if (R_FAILED(svcGetInfo(&progid, InfoType_ProgramId, CUR_PROCESS_HANDLE, 0)))
			progid = 0;

		return progid;
	}

	GameType RuntimeGame() {
		static GameType actualGame = GameType::Invalid;

		if (actualGame != GameType::Invalid)
			return actualGame;

		switch(RuntimeProgramID()) {
			case 0x0100E95004038000:
			case 0x0100F3400332C000: // JP version
				actualGame = GameType::BF2;
				break;
			case 0x0100C9F009F7A000:
				actualGame = GameType::IRA;
				break;
			case 0x0100FF500E34A000:
				actualGame = GameType::BFSW;
				break;
			case 0x010074F013262000:
				actualGame = GameType::BF3;
				break;
			default:
				actualGame = GameType::Invalid;
				break;
		}

		return actualGame;
	}

	SemVer RuntimeVersion() {
		static SemVer semver {};

		if (semver.IsValid())
			return semver;

		nn::oe::DisplayVersion nnVer {};
		nn::oe::GetDisplayVersion(&nnVer);

		if (std::sscanf(nnVer.name, "%d.%d.%d", &semver.major, &semver.minor, &semver.patch) == 3)
			return semver;

		return {0,0,0};
	}

} // namespace xenomods::version