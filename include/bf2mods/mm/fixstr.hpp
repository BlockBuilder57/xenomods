#pragma once

namespace mm {

	namespace mtl {

		template<std::size_t size>
		class FixStr {
		   public:
			char buffer[size];
			std::size_t m_nLen;

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
		};

	} // namespace mtl

} // namespace mm