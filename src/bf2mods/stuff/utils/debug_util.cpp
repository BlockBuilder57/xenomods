// moved into a new TU because it should have been that way in the first place

#include <bf2mods/stuff/utils/debug_util.hpp>

#include <cxxabi.h>

#include <list>
#include <sstream>
#include <unordered_map>

#include <bf2mods/stuff/utils/hid.hpp>
#include "nn/diag.h"
#include "nn/hid.hpp"
#include "skyline/logger/Logger.hpp"
#include "skyline/nx/kernel/svc.h"
#include "skyline/utils/cpputils.hpp"

extern "C" uint64_t __module_start;

namespace dbgutil {

	std::array<uintptr_t, MAX_TRACE_SIZE> getStackTrace() {
		auto result = std::array<uintptr_t, MAX_TRACE_SIZE>{0};
		nn::diag::GetBacktrace(result.data(), MAX_TRACE_SIZE);
		return result;
	}

	std::string getSymbol(uintptr_t address) {
		std::array<char, 0x100> symbolStrBuffer{0};
		nn::diag::GetSymbolName(symbolStrBuffer.data(), symbolStrBuffer.size(), address);

		if (!(strlen(symbolStrBuffer.data()) > 0)) {
			return "";
		}

		auto symbolAddress = uintptr_t{};
		if (R_FAILED(nn::ro::LookupSymbol(&symbolAddress, symbolStrBuffer.data()))) {
			return "nn::ro::LookupSymbol failed";
		}

		int rc;
		auto demangledStrBuffer = abi::__cxa_demangle(symbolStrBuffer.data(), nullptr, nullptr, &rc);

		switch (rc) {
			case 0: // no error
				// abi::__cxa_demangle succeeded, so we copy the data from the buffer it returned
				// and then free() as ABI docs say to do.
				strncpy(symbolStrBuffer.data(), demangledStrBuffer, symbolStrBuffer.size() - 1);
				free(demangledStrBuffer);
				break;
			case -1: // malloc failure
				return "abi::__cxa_demangle unable to allocate memory";
			case -2: // not valid name, just use the mangled name then
				break;
			case -3: // invalid argument(s)
				return "invalid arguments to abi::__cxa_demangle?";
		}

		std::stringstream resultSs;
		resultSs << symbolStrBuffer.data() << "+" << std::hex << address - symbolAddress;

		return resultSs.str();
	}

	void logStackTrace() {
		for (auto address : getStackTrace()) {
			if (!address) {
				break;
			}

			auto symbolStrBuffer = getSymbol(address);

			if ((size_t) address > (size_t) &__module_start) {
				LOG("skyline+%lx %s", (size_t) address - (size_t) &__module_start, symbolStrBuffer.data());
			} else if ((size_t) address > skyline::utils::g_MainTextAddr) {
				LOG("%lx %s", (size_t) address - skyline::utils::g_MainTextAddr + TEXT_OFFSET, symbolStrBuffer.data());
			} else {
				LOG("main-%lx %s", skyline::utils::g_MainTextAddr - (size_t) address, symbolStrBuffer.data());
			}
		}
	}

	void logRegistersX(InlineCtx* ctx) {
		constexpr auto REGISTER_COUNT = sizeof(ctx->registers) / sizeof(ctx->registers[0]);

		for (auto i = 0u; i < REGISTER_COUNT; i++) {
			LOG("X%d: %lx", i, ctx->registers[i].x);
		}
	}

	void logMemory(void* address, size_t len) {
#ifndef NOLOG
		static const char NIBBLE_LOOKUP[] = {'0', '1', '2', '3', '4', '5', '6', '7',
		                                     '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

		uint8_t* addressAsBytes = static_cast<uint8_t*>(address);
		char printBuffer[len * 3];
		for (auto i = 0u; i < len; i++) {
			auto printBufferOffset = i * 3;
			printBuffer[printBufferOffset] = NIBBLE_LOOKUP[addressAsBytes[i] >> 4];
			printBuffer[printBufferOffset + 1] = NIBBLE_LOOKUP[addressAsBytes[i] & 0xF];
			printBuffer[printBufferOffset + 2] = ' ';
		}
		printBuffer[len * 3 - 1] = '\0';

		LOG("%s", printBuffer);
#endif
	}

	void poorPersonsBreakpoint(std::string msg) {
#ifdef NOLOG
		return;
#endif
		constexpr auto CONTINUE_ONCE_KEY = nn::hid::KEY_ZR;
		constexpr auto CONTINUE_HOLD_KEY = nn::hid::KEY_ZL;
		constexpr auto LOG_BT_KEY = nn::hid::KEY_R;
		constexpr auto DISABLE_BREAK_POINTS_KEY = nn::hid::KEY_LSTICK | nn::hid::KEY_RSTICK;

		static auto breakpointIsDisabled = false;
		if (breakpointIsDisabled) {
			return;
		}

		auto npadScanner = util::NpadScanner{.useHandheldStyle = true, .npadId = nn::hid::CONTROLLER_PLAYER_1};
		npadScanner.scanInput(); // prepare privButtons

		LOG("breakpoint reached: %s", msg.c_str());

		while (true) {
			npadScanner.scanInput();

			if (npadScanner.keyState.Buttons & CONTINUE_HOLD_KEY) {
				LOG("breakpoint ignored");
				break;
			}

			if (npadScanner.keyComboJustPressed(CONTINUE_ONCE_KEY)) {
				LOG("breakpoint continued");
				break;
			}

			if (npadScanner.keyComboJustPressed(DISABLE_BREAK_POINTS_KEY)) {
				LOG("all breakpoints disabled");
				breakpointIsDisabled = true;
				break;
			}

			if (npadScanner.keyComboJustPressed(LOG_BT_KEY)) {
				logStackTrace();
			}

			svcSleepThread(20000000);
		}
	}

	// file watch
	static std::unordered_map<void*, std::string> s_fileWatchMap = {};

	void addFileHandleToWatch(nn::fs::FileHandle fileHandle, const char* path) {
		s_fileWatchMap[fileHandle.handle] = path;
	}

	void removeFileHandleFromWatch(nn::fs::FileHandle fileHandle) {
		s_fileWatchMap.erase(fileHandle.handle);
	}

	bool handleIsWatched(nn::fs::FileHandle fileHandle) {
		return s_fileWatchMap.find(fileHandle.handle) != end(s_fileWatchMap);
	}

	std::string getHandlePath(nn::fs::FileHandle fileHandle) {
		return s_fileWatchMap[fileHandle.handle];
	}

} // namespace dbgutil