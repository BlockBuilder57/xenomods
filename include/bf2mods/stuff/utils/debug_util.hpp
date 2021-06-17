#ifndef STUFF_DEBUG_UTIL
#define STUFF_DEBUG_UTIL

#include <string>
#include <array>

#include "hid.hpp"
#include "nn/diag.h"
#include "nn/hid.hpp"
#include "skyline/logger/Logger.hpp"
#include "skyline/nx/kernel/svc.h"
#include "skyline/utils/cpputils.hpp"
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

	std::string getSymbol(uintptr_t address);

	void logStackTrace();


	void logRegistersX(InlineCtx* ctx);

	void logMemory(void* address, size_t len);

	void poorPersonsBreakpoint(std::string msg);


	void addFileHandleToWatch(nn::fs::FileHandle fileHandle, const char* path);

	void removeFileHandleFromWatch(nn::fs::FileHandle fileHandle);

	bool handleIsWatched(nn::fs::FileHandle fileHandle);

	std::string getHandlePath(nn::fs::FileHandle fileHandle);

} // namespace dbgutil

#endif
