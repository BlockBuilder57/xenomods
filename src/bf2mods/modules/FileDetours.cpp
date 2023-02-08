//
// Created by block on 1/9/2023.
//

#include "FileDetours.hpp"

#include "../main.hpp"
#include "bf2mods/DebugWrappers.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/NnFile.hpp"
#include "bf2mods/Utils.hpp"
#include "bf2mods/engine/ml/Filesystem.hpp"
#include "bf2mods/engine/mm/FixStr.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/stuff/utils/util.hpp"

#include <skylaunch/hookng/Hooks.hpp>

namespace {

	void CleanPath(std::string& path, bool flat = false) {
		// https://en.cppreference.com/w/cpp/string/basic_string/replace
		auto replace_all = [](std::string& inout, std::string_view what, std::string_view with) {
			std::size_t count {};
			for(std::string::size_type pos {};
				inout.npos != (pos = inout.find(what.data(), pos, what.length()));
				pos += with.length(), ++count) {
				inout.replace(pos, what.length(), with.data(), with.length());
			}
			return count;
		};

		if(path.starts_with("/"))
			path.erase(0, 1);

		if(flat)
			replace_all(path, "/", "_");
		replace_all(path, ":", "_");
		replace_all(path, "//", "/");
	}

	bool EnsurePath(std::string& path, bool createPaths = true) {
		// https://stackoverflow.com/a/44495206
		auto split = [](const std::string& input, std::string_view delimiter) {
			size_t pos_start = 0, pos_end, delim_len = delimiter.length();
			std::string token;
			std::vector<std::string> res;

			while((pos_end = input.find(delimiter, pos_start)) != std::string::npos) {
				token = input.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.push_back(token);
			}

			res.push_back(input.substr(pos_start));
			return res;
		};

		auto splits = split(path, "/");

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
							bf2mods::g_Logger->LogError("Failed to create dump directory \"{}\" for reason {}", ss.str(), res);
							return false;
						}
					} else {
						bf2mods::g_Logger->LogError("Failed to open dump directory \"{}\" for reason {}", ss.str(), res);
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
		if(!bf2mods::NnFile::Preallocate(path, length)) {
			bf2mods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\"", path);
		}

		bf2mods::NnFile file(path, nn::fs::OpenMode_Write);

		if(!file) {
			bf2mods::g_Logger->LogError("Couldn't open dump file \"{}\"", path);
			return false;
		}

		file.Write(buffer, length);
		file.Flush();
		return true;
	}

	void LoadFromFilesystem(std::string_view jackPath, void* buffer, std::size_t length) {
		bf2mods::NnFile file(jackPath, nn::fs::OpenMode_Read);

		// This isn't really fatal since the override file
		// doesn't particularly need to exist. It's whatever really
		if(!file)
			return;

		file.Read(buffer, file.Size());
		bf2mods::g_Logger->LogDebug("Loaded dump file \"{}\"", jackPath);
	}

	void FileDetourImpl(ml::FileHandleTh* fileHandle, ml::FileReadResult& readResult) {
		auto filename = std::string(fileHandle->filename.buffer);
		CleanPath(filename);

		// dump data reads to sd card
		if(bf2mods::GetState().config.dumpFileReads) {
			auto path = fmt::format("sd:/config/bf2mods/{}/dump/{}/{:08x}.bin", BF2MODS_CODENAME_STR, filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			if(EnsurePath(path, true))
				DumpToFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}

		// load from loose sd card files
		if(bf2mods::GetState().config.useFileDumps) {
			auto path = fmt::format("sd:/config/bf2mods/{}/override/{}/{:08x}.bin", BF2MODS_CODENAME_STR, filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			LoadFromFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}
	}

	struct ReadFileHook : skylaunch::hook::Trampoline<ReadFileHook> {
		static uint Hook(ml::FileHandleTh*& fh, nn::fs::FileHandle& nnHandle, int unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
			// used by maps (exclusively?)

			auto fileRes = Orig(fh, nnHandle, unk, offset, buffer, buffersize, readres);
			//bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFile: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

			if(fileRes && readres.bFullyLoaded == 1)
				FileDetourImpl(fh, readres);

			return fileRes;
		}
	};

	struct ReadFileSliceHook : skylaunch::hook::Trampoline<ReadFileSliceHook> {
		static uint Hook(ml::FileHandleTh* fh, nn::fs::FileHandle& nnHandle, long unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
			// used by most other things

			auto fileRes = Orig(fh, nnHandle, unk, offset, buffer, buffersize, readres);
			//bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFileSlice: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

			if(fileRes && readres.bFullyLoaded == 1)
				FileDetourImpl(fh, readres);

			return fileRes;
		}
	};

} // namespace

namespace bf2mods {

	void FileDetours::Initialize() {
		g_Logger->LogDebug("Setting up file detours...");

		ReadFileHook::HookAt("_ZN2ml13DevFileUtilNx8readFileERPNS_12FileHandleThERN2nn2fs10FileHandleEilPvjRNS_14FileReadResultE");
		ReadFileSliceHook::HookAt("_ZN2ml13DevFileUtilNx13readFileSliceEPNS_12FileHandleThERN2nn2fs10FileHandleEllPvjRNS_14FileReadResultE");
	}

	BF2MODS_REGISTER_MODULE(FileDetours);

} // namespace bf2mods
