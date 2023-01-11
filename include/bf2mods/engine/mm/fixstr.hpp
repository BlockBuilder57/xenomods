#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>

namespace mm {

	namespace mtl {

		template<int32_t size>
		class [[gnu::packed]] FixStr {
		   public:
			char buffer[size];
			std::uint32_t m_nLen;

			/*char* format (char* fmt, ...) {
				va_list args;
				va_start(args, fmt);
				vsnprintf(buffer, 256, fmt, args);
				va_end(args);

				m_nLen = strlen(buffer);
				assert(m_nLen < size-1);

				strcpy(fmt, buffer);
				return fmt;
			}*/

			void set(const char* text) {
				int length = strlen(text);
				memcpy(&buffer[0], text, length);
			}
		};

	} // namespace mtl

} // namespace mm

namespace bf2mods {
	template<int32_t N>
	std::string_view GetStringView(const mm::mtl::FixStr<N>& str) {
		auto len = str.m_nLen - 1;

		// for some reason this can get screwy...
		if(len > N)
			len = strlen(str.buffer);

		return { str.buffer, len };
	}
} // namespace bf2mods