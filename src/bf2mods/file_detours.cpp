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

	void CleanPath(std::string& path, bool flat = false) {;
		// https://en.cppreference.com/w/cpp/string/basic_string/replace
		auto replace_all = [](std::string& inout, std::string_view what, std::string_view with) {
			std::size_t count{};
			for (std::string::size_type pos{};
				inout.npos != (pos = inout.find(what.data(), pos, what.length()));
				pos += with.length(), ++count) {
				inout.replace(pos, what.length(), with.data(), with.length());
			}
			return count;
		};

		if (path.starts_with("/"))
			path.erase(0, 1);

		if (flat)
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

		for (int i = 0; i < splits.size(); i++) {
			ss << splits[i];

			if (i != splits.size() - 1) {
				ss << "/";

				Result res{};
				nn::fs::DirectoryHandle dh{};
				res = nn::fs::OpenDirectory(&dh, ss.str().c_str(), nn::fs::OpenDirectoryMode::OpenDirectoryMode_All);

				if (R_FAILED(res)) {
					if (res == 0x202 && createPaths) { // Path does not exist
						res = nn::fs::CreateDirectory(ss.str().c_str());
						if (R_FAILED(res) && res != 0x402) {
							bf2mods::g_Logger->LogError("Failed to create dump directory \"{}\" for reason {}", ss.str(), res);
							return false;
						}
					}
					else {
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


	// TODO: new file please !

	/**
	 * A really stupid wrapper over nn filesystem functions
	 */
	struct NnFile {

		explicit NnFile(std::string_view path, nn::fs::OpenMode mode) {
			Open(path, mode);
		}

		~NnFile() {
			Close();
		}

		void Open(std::string_view path, nn::fs::OpenMode mode) {
			if(R_FAILED(nn::fs::OpenFile(&fh, path.data(), mode))) {
				fh.handle = reinterpret_cast<void*>(-1);
			} else {
				// I give up
				if(R_FAILED(nn::fs::GetFileSize(&size, fh)))
					Close();
			}
		}

		void Close() {
			if(Ok()) {
				nn::fs::CloseFile(fh);
				fh.handle = reinterpret_cast<void*>(-1);
			}

		}

		bool Ok() const {
			return fh.handle != reinterpret_cast<void*>(-1);
		}

		operator bool() const {
			return Ok();
		}

		void Read(void* buffer, s64 length) {
			if(!Ok())
				return;

			if(R_SUCCEEDED(nn::fs::ReadFile(fh, filePointer, buffer, length))) {
				// this is dirty but the minimum sdk version doesn't have the overload which can actually
				// signal short reads.... so this will have to do. i guess i could do some min() stuff
				filePointer += length;
			}
		}

		void Write(const void* buffer, s64 length) {
			if(!Ok())
				return;

			if(R_SUCCEEDED(nn::fs::WriteFile(fh, filePointer, buffer, length, {}))) {
				filePointer += length;
			}
		}

		void Flush() {
			if(!Ok())
				return;

			nn::fs::FlushFile(fh);
		}

		void Seek(s64 offset, int whence) {
			switch(whence) {
				case SEEK_SET: filePointer = offset; break;
				case SEEK_CUR: filePointer += offset; break;
				case SEEK_END: filePointer = size + offset; break; // i think ?
				default: return;
			}
		}

		s64 Tell() const {
			return filePointer;
		}

		s64 Size() const {
			return size;
		}

		/**
		 * Preallocate a file with the given size.
		 *
		 * \param[in] path path to file to preallocate
		 * \param[in] size the file size
		 * \return true if able to preallocate; false otherwise
		 */
		static bool Preallocate(std::string_view path, s64 size) {
			auto res = nn::fs::CreateFile(path.data(), size);
			if(R_FAILED(res)) {

				if(R_VALUE(res) == 0x0402) {
					u64 fileSize;
					{
						NnFile file(path, nn::fs::OpenMode_Read);
						fileSize = file.Size();
					}

					if(fileSize != size) {
						nn::fs::DeleteFile(path.data());

						// Yay for recursion!
						return Preallocate(path, size);
					} else {
						// seems to be the right size so let's not bother
						// trying to recurse in pedantry's sake
						return true;
					}
				}

				return false;
			}

			return true;
		}

	   private:

		nn::fs::FileHandle fh{};
		nn::fs::OpenMode mode{};

		s64 filePointer{};
		s64 size{};
	};

	bool DumpToFilesystem(std::string_view path, const void* buffer, std::size_t length) {
		Result res{};
		nn::fs::FileHandle dumpFileHandle{};


		if(!NnFile::Preallocate(path, length)) {
			bf2mods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\": 0x{:08x};", path, res);
		}

		NnFile file(path, nn::fs::OpenMode_Write);

		if(!file) {
			bf2mods::g_Logger->LogError("Couldn't open dump file \"{}\": 0x{:08x}", path, res);
			return false;
		}

		file.Write(buffer, length);
		file.Flush();
		return true;
	}


	void LoadFromFilesystem(std::string_view jackPath, void* buffer, std::size_t length) {
		NnFile file(jackPath, nn::fs::OpenMode_Read);

		// This isn't really fatal since the override file
		// doesn't particularly need to exist. It's whatever really
		if(!file)
			return;

		file.Read(buffer, file.Size());
		bf2mods::g_Logger->LogDebug("Loaded dump file \"{}\".", jackPath);
	}

	void FileDetourImpl(ml::FileHandleTh* fileHandle, ml::FileReadResult& readResult) {
		auto filename = std::string(fileHandle->filename.buffer);
		CleanPath(filename);

		// dump data reads to sd card
		if (bf2mods::GetState().config.dumpFileReads) {
			auto path = fmt::format("sd:/config/bf2mods/dump/{}/{:08x}.bin", filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			if (EnsurePath(path, true))
				DumpToFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}

		// load from loose sd card files
		if (bf2mods::GetState().config.useFileDumps) {
			auto path = fmt::format("sd:/config/bf2mods/override/{}/{:08x}.bin", filename, reinterpret_cast<uint32_t>(fileHandle->readStartOffset));
			LoadFromFilesystem(path, fileHandle->mMemBuffer, readResult.bytesRead);
		}

	}

}

namespace ml {

	GENERATE_SYM_HOOK(DevFileUtilNx_readFile, "_ZN2ml13DevFileUtilNx8readFileERPNS_12FileHandleThERN2nn2fs10FileHandleEilPvjRNS_14FileReadResultE", uint, ml::FileHandleTh*& fh, nn::fs::FileHandle& nnHandle, int unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
		// used by maps (exclusively?)

		auto fileRes = DevFileUtilNx_readFileBak(fh, nnHandle, unk, offset, buffer, buffersize, readres);
		//bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFile: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

		if(fileRes && readres.bFullyLoaded == 1)
			FileDetourImpl(fh, readres);

		return fileRes;
	}

	GENERATE_SYM_HOOK(DevFileUtilNx_readFileSlice, "_ZN2ml13DevFileUtilNx13readFileSliceEPNS_12FileHandleThERN2nn2fs10FileHandleEllPvjRNS_14FileReadResultE", uint, ml::FileHandleTh* fh, nn::fs::FileHandle& nnHandle, long unk, long offset, void* buffer, unsigned int buffersize, ml::FileReadResult& readres) {
		// used by most other things

		auto fileRes = DevFileUtilNx_readFileSliceBak(fh, nnHandle, unk, offset, buffer, buffersize, readres);
		//bf2mods::g_Logger->LogDebug("DevFileUtilNx::readFileSlice: \"{}\" (start offset 0x{:08x} size 0x{:08x}, was compressed? {})", fh->filename.buffer, fh->readStartOffset, fh->readSize, readres.bWasCompressed);

		if(fileRes && readres.bFullyLoaded == 1)
			FileDetourImpl(fh, readres);

		return fileRes;
	}

} // namespace ml



namespace bf2mods {

	void FileDetours::Initialize() {
		g_Logger->LogDebug("Setting up file detours...");

		ml::DevFileUtilNx_readFileHook();
		ml::DevFileUtilNx_readFileSliceHook();
	}

	BF2MODS_REGISTER_MODULE(FileDetours);

} // namespace bf2mods
