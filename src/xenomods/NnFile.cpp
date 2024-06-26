#include <xenomods/NnFile.hpp>

namespace xenomods {

	NnFile::~NnFile() {
		Close();
	}

	void NnFile::Open(std::string_view path, nn::fs::OpenMode openMode) {
		if(R_FAILED(nn::fs::OpenFile(&fh, path.data(), openMode))) {
			fh.handle = std::bit_cast<void*>(-1ull);
		} else {
			mode = openMode;
			// I give up
			if(R_FAILED(nn::fs::GetFileSize(&size, fh)))
				Close();
		}
	}

	void NnFile::Close() {
		if(Ok()) {
			nn::fs::CloseFile(fh);
			fh.handle = std::bit_cast<void*>(-1ull);
		}
	}

	bool NnFile::Read(void* buffer, s64 length) {
		if(!Ok())
			return false;

		if(R_SUCCEEDED(nn::fs::ReadFile(fh, filePointer, buffer, length))) {
			// this is dirty but the minimum sdk version doesn't have the overload which can actually
			// signal short reads.... so this will have to do. i guess i could do some min() stuff
			filePointer += length;
		}

		return true;
	}

	void NnFile::Write(const void* buffer, s64 length) {
		if(!Ok())
			return;

		if(R_SUCCEEDED(nn::fs::WriteFile(fh, filePointer, buffer, length, {}))) {
			filePointer += length;
		}
	}

	void NnFile::Flush() {
		if(!Ok())
			return;

		nn::fs::FlushFile(fh);
	}

	void NnFile::Seek(s64 offset, SeekDirection whence) {
		using enum SeekDirection;
		switch(whence) {
			case Beg:
				filePointer = offset;
				break;
			case Cur:
				filePointer += offset;
				break;
			case End:
				filePointer = size + offset;
				break;
			default:
				return;
		}
	}

	bool NnFile::Preallocate(std::string_view path, s64 size) {
		NnFile::EnsurePath(path);

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

	u64 NnFile::GetLastModified(const char* path) {
		nn::fs::FileTimeStamp timestamp {};
		if(R_FAILED(nn::fs::GetFileTimeStampForDebug(&timestamp, path))) {
			return 0;
		}
		return timestamp.modified;
	}

	void NnFile::CleanPath(std::string& path, bool flat/* = false*/) {
		if(path.starts_with("/"))
			path.erase(0, 1);

		if(flat)
			xenomods::StringReplace(path, "/", "_");
		xenomods::StringReplace(path, ":", "_");
		xenomods::StringReplace(path, "//", "/");
	}

	bool NnFile::EnsurePath(std::string_view& path, bool createPaths/* = true*/) {
		auto splits = xenomods::StringSplit(path.data(), "/");

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

} // namespace xenomods
