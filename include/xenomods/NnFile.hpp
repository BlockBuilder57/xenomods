#pragma once

#include <xenomods/Logger.hpp>
#include <nn/fs.h>

#include <bit>
#include <string_view>

namespace xenomods {

	/**
	 * A file object backed via nn::fs
	 */
	struct NnFile {
		enum class SeekDirection : u8 {
			Beg, ///< Relative to the beginning of the file
			Cur, ///< Relative to the current file pointer.
			End	 ///< Relative to the end of the file (use negative offsets).
		};

		NnFile() = default;

		/**
		 * Shorthand constructor to open file.
		 *
		 * \param[in] path     Path to file to open.
		 * \param[in] openMode Desired open mode.
		 */
		explicit NnFile(std::string_view path, nn::fs::OpenMode openMode) {
			Open(path, openMode);
		}

		~NnFile();

		/**
		 * Open the specified file.
		 *
		 * \param[in] path     Path to file to open.
		 * \param[in] openMode Desired open mode.
		 */
		void Open(std::string_view path, nn::fs::OpenMode openMode);

		/**
		 * Close the file.
		 */
		void Close();

		/**
		 * Check if file is valid.
		 *
		 * \returns True if valid, false if not.
		 */
		[[nodiscard]] constexpr bool Ok() const {
			return fh.handle != std::bit_cast<void*>(-1ull);
		}

		[[nodiscard]] constexpr operator bool() const {
			return Ok();
		}

		/**
		 * Read data from the file.
		 *
		 * \param[out] buffer The buffer to read into.
		 * \param[in]  length Size to read from the file.
		 */
		bool Read(void* buffer, s64 length);

		/**
		 * Read data from the file.
		 *
		 * \param[out] buffer The buffer to read into.
		 */
		template <typename T>
		bool ReadOne(T* buffer) {
			if(!Ok())
				return false;

			if(R_SUCCEEDED(nn::fs::ReadFile(fh, filePointer, buffer, sizeof(T)))) {
				// this is dirty but the minimum sdk version doesn't have the overload which can actually
				// signal short reads.... so this will have to do. i guess i could do some min() stuff
				filePointer += sizeof(T);
			}

			return true;
		}

		/**
		 * Write data into the file.
		 *
		 * \param[in] buffer The buffer to write into the file.
		 * \param[in]  length Size to write to the file.
		 */
		void Write(const void* buffer, s64 length);

		/**
		 * Flush any pending write operations.
		 */
		void Flush();

		/**
		 * Seek the file pointer.
		 *
		 * \param[in] offset The offset.
		 * \param[in] whence The direction to seek.
		 */
		void Seek(s64 offset, SeekDirection whence);

		/**
		 * Tell the current file pointer.
		 *
		 * \returns The file pointer.
		 */
		[[nodiscard]] constexpr s64 Tell() const {
			return filePointer;
		}

		/**
		 * Get the file size.
		 *
		 * \returns The file size.
		 */
		[[nodiscard]] constexpr s64 Size() const {
			return size;
		}

		/**
		 * Preallocate a file with the given size.
		 *
		 * \param[in] path path to file to preallocate
		 * \param[in] size the file size
		 * \return true if able to preallocate; false otherwise
		 */
		[[nodiscard]] static bool Preallocate(std::string_view path, s64 size);

		/**
		 * Ensures a path is clean of invalid characters.
		 * Specifically, /'s and :'s are replaced with _, and // is replaced with /.
		 *
		 * \param[in] path The path to clean.
		 * \param[in] flat If enabled, folder separators are replaced with _.
		 */
		static void CleanPath(std::string& path, bool flat = false);

		/**
		 * Ensures the directory structure up to this point.
		 *
		 * \param[in] path The path to ensure.
		 * \param[in] createPaths Create the paths, or just return false?
		 * \return true If the path is (now) valid.
		 */
		static bool EnsurePath(std::string_view& path, bool createPaths = true);

		/**
		 * Get the time the file was last modified.
		 *
		 * \param[in] path the full path to the file
		 * \return the POSIX time this file was last modified, or 0
		 * if it could not be fetched.
		 */
		static u64 GetLastModified(const char* path);

	   private:
		nn::fs::FileHandle fh {};
		nn::fs::OpenMode mode {};

		s64 filePointer {};
		s64 size {};
	};

} // namespace xenomods
