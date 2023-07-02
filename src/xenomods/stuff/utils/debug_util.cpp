// moved into a new TU because it should have been that way in the first place

#include <cxxabi.h>

#include <xenomods/stuff/utils/debug_util.hpp>
#include <xenomods/NnFile.hpp>
#include <list>
#include <sstream>
#include <unordered_map>

#include "nn/diag.h"
#include "nn/hid.hpp"
#include "skylaunch/logger/Logger.hpp"
#include "skylaunch/nx/kernel/svc.h"
#include "skylaunch/utils/cpputils.hpp"

#include <fmt/core.h>

extern "C" uint64_t __module_start;

namespace dbgutil {

	std::array<uintptr_t, MAX_TRACE_SIZE> getStackTrace() {
		auto result = std::array<uintptr_t, MAX_TRACE_SIZE> { 0 };
		nn::diag::GetBacktrace(result.data(), MAX_TRACE_SIZE);
		return result;
	}

	std::string getSymbol(uintptr_t address, bool leaveMangled) {
		std::array<char, 0x100> symbolStrBuffer { 0 };
		auto symbolAddress = uintptr_t {};

		nn::diag::GetSymbolName(symbolStrBuffer.data(), symbolStrBuffer.size(), address);

		auto mangledLength = strlen(symbolStrBuffer.data());

		if(mangledLength <= 0) {
			return fmt::format("{:#016x} (no symbol name)", address);
		}

		if (leaveMangled)
			return std::string{ symbolStrBuffer.data(), mangledLength };

		Result res{};
		if(R_FAILED(res = nn::ro::LookupSymbol(&symbolAddress, symbolStrBuffer.data())))
			return fmt::format("nn::ro::LookupSymbol failed (result {:#x})", res);

		int demangleRc;
		size_t demangledLength = symbolStrBuffer.size();

		abi::__cxa_demangle(symbolStrBuffer.data(), symbolStrBuffer.data(), &demangledLength, &demangleRc);

		// if it fails with anything other than -2 (invalid name)
		// give up
		if(demangleRc != 0 && demangleRc != -2)
			return fmt::format("error in abi::__cxa_demangle: {}", demangleRc);

		if (address - symbolAddress != 0)
			return fmt::format("{}+{:#x}", std::string_view { symbolStrBuffer.data(), demangledLength }, (address - symbolAddress));

		return std::string{ symbolStrBuffer.data(), demangledLength };
	}

	void logStackTrace() {
		for(auto address : getStackTrace()) {
			if(!address) {
				break;
			}

			auto symbolStrBuffer = getSymbol(address);

			if((size_t)address > (size_t)&__module_start) {
				LOG("skylaunch+{:#x} {}", (size_t)address - (size_t)&__module_start, symbolStrBuffer.data());
			} else if((size_t)address > skylaunch::utils::g_MainTextAddr) {
				LOG("{:#x} {}", (size_t)address - skylaunch::utils::g_MainTextAddr + TEXT_OFFSET, symbolStrBuffer.data());
			} else {
				LOG("main-{:#x} {}", skylaunch::utils::g_MainTextAddr - (size_t)address, symbolStrBuffer.data());
			}
		}
	}

	void logRegistersX(InlineCtx* ctx) {
		constexpr auto REGISTER_COUNT = sizeof(ctx->registers) / sizeof(ctx->registers[0]);

		for(auto i = 0u; i < REGISTER_COUNT; i++) {
			LOG("X{}: {:lx}", i, ctx->registers[i].x);
		}
	}

	void logMemory(void* address, size_t len) {
#ifndef NOLOG
		static constexpr char NIBBLE_LOOKUP[] = "0123456789ABCDEF";

		NN_DIAG_LOG(nn::diag::LogSeverity::Info, "Memory @ %p - size %x", address, len);

		uint8_t* addressAsBytes = static_cast<uint8_t*>(address);
		char printBuffer[len * 3];
		memset(&printBuffer, 0, sizeof(printBuffer));
		for(auto i = 0u; i < len; i++) {
			auto printBufferOffset = (i % 0x20) * 3;
			printBuffer[printBufferOffset] = NIBBLE_LOOKUP[addressAsBytes[i] >> 4];
			printBuffer[printBufferOffset + 1] = NIBBLE_LOOKUP[addressAsBytes[i] & 0xF];
			printBuffer[printBufferOffset + 2] = ' ';
			if (printBufferOffset == (0x20-1) * 3) {
				printBuffer[printBufferOffset + 3] = '\0';
				NN_DIAG_LOG(nn::diag::LogSeverity::Info, "%s", printBuffer);
				memset(&printBuffer, 0, sizeof(printBuffer));
			}
		}
		//printBuffer[len * 3 - 1] = '\0';

		//LOG("{}", printBuffer);
#endif
	}

	void dumpMemory(void* address, size_t len, const char* name /*= nullptr*/) {
		std::string path = "sd:/config/xenomods/memdump/";
		if (name == nullptr)
			path += fmt::format("{:08x}.dump", nn::os::GetSystemTick() & 0xFFFFFFFF);
		else
			path += std::string_view(name);

		if(!xenomods::NnFile::Preallocate(path, len)) {
			xenomods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\"", path);
			return;
		}

		xenomods::NnFile file(path, nn::fs::OpenMode_Write);

		if(!file) {
			xenomods::g_Logger->LogError("Couldn't open dump file \"{}\"", path);
			return;
		}

		file.Write(address, len);
		file.Flush();
	}

	void loadMemory(void* address, std::string path) {
		xenomods::NnFile file(path, nn::fs::OpenMode_Read);

		if(!file) {
			xenomods::g_Logger->LogError("Couldn't open dump file \"{}\"", path);
			return;
		}

		file.Read(address, file.Size());
	}

} // namespace dbgutil