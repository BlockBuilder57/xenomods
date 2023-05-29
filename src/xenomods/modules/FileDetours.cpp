//
// Created by block on 1/9/2023.
//

#include "FileDetours.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>

#include "xenomods/engine/ml/Filesystem.hpp"
#include "xenomods/engine/mm/FixStr.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	void CleanPath(std::string& path, bool flat = false) {
		if(path.starts_with("/"))
			path.erase(0, 1);

		if(flat)
			xenomods::StringReplace(path, "/", "_");
		xenomods::StringReplace(path, ":", "_");
		xenomods::StringReplace(path, "//", "/");
	}

	bool EnsurePath(std::string& path, bool createPaths = true) {
		auto splits = xenomods::StringSplit(path, "/");

		std::stringstream ss;

		for(int i = 0; i < splits.size(); i++) {
			ss << splits[i];

			if(i != splits.size() - 1) {
				ss << "/";

				Result res {};
				nn::fs::DirectoryHandle dh {};
				res = nn::fs::OpenDirectory(&dh, ss.str().c_str(), nn::fs::OpenDirectoryMode::OpenDirectoryMode_All);

				if(R_FAILED(res)) {
					if(res == 0x202 && createPaths) { // Path does not exist
						res = nn::fs::CreateDirectory(ss.str().c_str());
						if(R_FAILED(res) && res != 0x402) {
							xenomods::g_Logger->LogError("Failed to create dump directory \"{}\" for reason {}", ss.str(), res);
							return false;
						}
					} else {
						xenomods::g_Logger->LogError("Failed to open dump directory \"{}\" for reason {}", ss.str(), res);
						return false;
					}
				} else {
					nn::fs::CloseDirectory(dh);
				}
			}
		}

		return true;
	}

	bool DumpToFilesystem(std::string_view path, const void* buffer, std::size_t length) {
		if(!xenomods::NnFile::Preallocate(path, length)) {
			xenomods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\"", path);
		}

		xenomods::NnFile file(path, nn::fs::OpenMode_Write);

		if(!file) {
			xenomods::g_Logger->LogError("Couldn't open dump file \"{}\"", path);
			return false;
		}

		file.Write(buffer, length);
		file.Flush();
		return true;
	}

	void LoadFromFilesystem(std::string_view jackPath, void* buffer, std::size_t length) {
		xenomods::NnFile file(jackPath, nn::fs::OpenMode_Read);

		// This isn't really fatal since the override file
		// doesn't particularly need to exist. It's whatever really
		if(!file)
			return;

		file.Read(buffer, file.Size());
		xenomods::g_Logger->LogDebug("Loaded dump file \"{}\"", jackPath);
	}

	void FileDetourImpl(ml::FileHandleTh* fileHandle, ml::FileReadResult& readResult) {
		auto filename = std::string(fileHandle->filename.buffer);
		CleanPath(filename);

		// dump data reads to sd card
		if(xenomods::GetState().config.dumpFileReads) {
			auto path = fmt::format("sd:/config/xenomods/{}/dump/{}/{:08x}.bin", XENOMODS_CODENAME_STR, filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			if(EnsurePath(path, true))
				DumpToFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}

		// load from loose sd card files
		if(xenomods::GetState().config.enableFileOverrides) {
			auto path = fmt::format("sd:/config/xenomods/{}/override/{}/{:08x}.bin", XENOMODS_CODENAME_STR, filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			LoadFromFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}
	}

	struct ReadFileHook : skylaunch::hook::Trampoline<ReadFileHook> {
		static uint Hook(ml::FileHandleTh*& fh, nn::fs::FileHandle& nnHandle, int unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
			// used by maps (exclusively?)

			auto fileRes = Orig(fh, nnHandle, unk, offset, buffer, buffersize, readres);
			//xenomods::g_Logger->LogDebug("DevFileUtilNx::readFile: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

			if(fileRes && readres.bFullyLoaded == 1)
				FileDetourImpl(fh, readres);

			return fileRes;
		}
	};

	struct ReadFileSliceHook : skylaunch::hook::Trampoline<ReadFileSliceHook> {
		static uint Hook(ml::FileHandleTh* fh, nn::fs::FileHandle& nnHandle, long unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
			// used by most other things

			auto fileRes = Orig(fh, nnHandle, unk, offset, buffer, buffersize, readres);
			//xenomods::g_Logger->LogDebug("DevFileUtilNx::readFileSlice: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

			if(fileRes && readres.bFullyLoaded == 1)
				FileDetourImpl(fh, readres);

			return fileRes;
		}
	};

} // namespace

namespace xenomods {

	void FileDetours::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up file detours...");

#if !XENOMODS_CODENAME(bf3)
		ReadFileHook::HookAt("_ZN2ml13DevFileUtilNx8readFileERPNS_12FileHandleThERN2nn2fs10FileHandleEilPvjRNS_14FileReadResultE");
		ReadFileSliceHook::HookAt("_ZN2ml13DevFileUtilNx13readFileSliceEPNS_12FileHandleThERN2nn2fs10FileHandleEllPvjRNS_14FileReadResultE");
#else
		ReadFileHook::HookFromBase(0x71012562a4);
		ReadFileSliceHook::HookFromBase(0x7101254ab0);
#endif
	}

	XENOMODS_REGISTER_MODULE(FileDetours);

} // namespace xenomods
