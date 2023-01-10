/**
 * @file diag.h
 * @brief Module, logging, and symbol operations.
 */

#pragma once

#include "types.h"

namespace nn {
	namespace diag {
		enum class LogSeverity : u8 {
			Trace = 0,
			Info,
			Warn,
			Error,
			Fatal
		};

		struct LogMetaData {
			std::uint32_t lineNo;
			LogSeverity severity;
			const char* fileName;
			const char* function;
			const char* moduleName;
		};

		struct ModuleInfo {
			char* mPath;
			u64 mBaseAddr;
			u64 mSize;
		};

		namespace detail {
			// LOG
			void LogImpl(nn::diag::LogMetaData const&, char const*, ...);
			void AbortImpl(char const*, char const*, char const*, s32);
			void AbortImpl(char const*, char const*, char const*, int, Result);
		}; // namespace detail

#define NN_DIAG_LOG(severity_, format, ...) ({ nn::diag::LogMetaData metadata{ .lineNo = __LINE__, .severity = severity_, .fileName = __FILE__, .function = __FUNCTION__, .moduleName = ""  }; nn::diag::detail::LogImpl(metadata, format, ##__VA_ARGS__);  })

		// MODULE / SYMBOL
		u32* GetSymbolName(char* name, u64 nameSize, u64 addr);
		u64 GetRequiredBufferSizeForGetAllModuleInfo();
		s32 GetAllModuleInfo(nn::diag::ModuleInfo** out, void* buffer, u64 bufferSize);
		u64 GetSymbolSize(u64 addr);

		int GetBacktrace(uintptr_t* pOutArray, int arrayCountMax);
	}; // namespace diag
};	   // namespace nn
