#ifndef STUFF_DEBUG_UTIL
#define STUFF_DEBUG_UTIL

#include <array>
#include <string>

#include "nn/diag.h"
#include "nn/hid.hpp"
#include "skylaunch/logger/Logger.hpp"
#include "skylaunch/nx/kernel/svc.h"
#include "skylaunch/utils/cpputils.hpp"
#include "util.hpp"

extern "C" uint64_t __module_start;

namespace dbgutil {

	constexpr size_t TEXT_OFFSET = 0x7100000000;

	constexpr size_t MAX_TRACE_SIZE = 0x20;

	struct StackFrame {
		StackFrame* p_nextFrame;
		void* lr;
	};

	std::array<uintptr_t, MAX_TRACE_SIZE> getStackTrace();

	std::string getSymbol(uintptr_t address, bool leaveMangled = false);

	void logStackTrace();

	void logRegistersX(InlineCtx* ctx);

	void logMemory(void* address, size_t len);

	void dumpMemory(void* address, size_t len, const char* name = nullptr);

	void loadMemory(void* address, std::string path);

} // namespace dbgutil

#endif
