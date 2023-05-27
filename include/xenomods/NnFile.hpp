#pragma once

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
		void Read(void* buffer, s64 length);

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
