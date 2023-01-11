//
// Created by block on 1/9/2023.
//

#include "file_detours.hpp"

#include <bf2mods/engine/ml/files.hpp>
#include <bf2mods/engine/mm/fixstr.hpp>

#include "bf2logger.hpp"
#include "bf2mods/debug_wrappers.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin_main.hpp"

namespace {

	void CleanPath(std::string& path) {;
		auto replace_all = [](std::string& inout, std::string_view what, std::string_view with) {
			std::size_t count{};
			for (std::string::size_type pos{};
				inout.npos != (pos = inout.find(what.data(), pos, what.length()));
				pos += with.length(), ++count) {
				inout.replace(pos, what.length(), with.data(), with.length());
			}
			return count;
		};

		replace_all(path, "/", "_");
		replace_all(path, ":", "_");
	}

	bool DumpToFs(std::string_view path, const void* buffer, std::size_t length) {
		Result res{};
		nn::fs::FileHandle dumpFileHandle{};

		// Create the dump file
		res = nn::fs::CreateFile(path.data(), length);
		if(R_FAILED(res) && res != 0x00000402 /* path already exists */) {
			bf2mods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\": 0x{:08x};", path, res);
			return false;
		}

		res = nn::fs::OpenFile(&dumpFileHandle, path.data(), nn::fs::OpenMode_Write);
		if(R_SUCCEEDED(res)) {
			res = nn::fs::WriteFile(dumpFileHandle, 0, buffer, length, {});

			if(R_SUCCEEDED(res)) {
				nn::fs::FlushFile(dumpFileHandle);
			} else {
				bf2mods::g_Logger->LogError("Couldn't write dump file \"{}\": 0x{:08x}", path, res);
			}

			nn::fs::CloseFile(dumpFileHandle);
		} else {
			bf2mods::g_Logger->LogError("Couldn't open dump file \"{}\": 0x{:08x}", path, res);
			return false;
		}

		return true;
	}

}

namespace ml {

	GENERATE_SYM_HOOK(DevFileUtilNx_readFile, "_ZN2ml13DevFileUtilNx8readFileERPNS_12FileHandleThERN2nn2fs10FileHandleEilPvjRNS_14FileReadResultE", uint, ml::FileHandleTh*& fh, nn::fs::FileHandle& nnHandle, int unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
		// used by maps (exclusively?)

		auto fileRes = DevFileUtilNx_readFileBak(fh, nnHandle, unk, offset, buffer, buffersize, readres);
		bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFile: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

#if 0
		if (fileRes) {
			// dump data reads to sd card

			auto filename = std::string(fh->filename.buffer);
			CleanPath(filename);

			auto path = fmt::format("sd:/config/bf2mods/dumptest/{}_{:08x}{}", filename, reinterpret_cast<uint32_t>(fh->readStartOffset), readres.bWasCompressed == 0 ? ".raw" : ".bin");

			DumpToFs(path, fh->mMemBuffer, readres.bytesRead);
		}
#endif

		return fileRes;
	}

	GENERATE_SYM_HOOK(DevFileUtilNx_readFileSlice, "_ZN2ml13DevFileUtilNx13readFileSliceEPNS_12FileHandleThERN2nn2fs10FileHandleEllPvjRNS_14FileReadResultE", uint, ml::FileHandleTh* fh, nn::fs::FileHandle& nnHandle, long unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
		// used by most other things

		auto fileRes = DevFileUtilNx_readFileSliceBak(fh, nnHandle, unk, offset, buffer, buffersize, readres);
		//bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFileSlice: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

#if 1
		if (fileRes) {
			// dump data reads to sd card

			auto filename = std::string(fh->filename.buffer);
			CleanPath(filename);

			auto path = fmt::format("sd:/config/bf2mods/dumptest/{}_{:08x}{}", filename, reinterpret_cast<uint32_t>(fh->readStartOffset), readres.bWasCompressed == 0 ? ".raw" : ".bin");

			DumpToFs(path, fh->mMemBuffer, readres.bytesRead);
		}
#endif

		return fileRes;
	}

} // namespace ml



namespace bf2mods {

	void FileDetours::Initialize() {
		g_Logger->LogDebug("Setting up file detours...");

		//ml::DrMdlMan_createMdlHook();
		//ml::DrMdlMan_createInstantMdlHook();

		//ml::DevFileTh_readFileHook();
		//ml::DevFileUtilNx_readFileHook();
		ml::DevFileUtilNx_readFileSliceHook();

	}

	BF2MODS_REGISTER_MODULE(FileDetours);

} // namespace bf2mods
