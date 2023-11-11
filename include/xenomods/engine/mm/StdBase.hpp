#pragma once


namespace mm {

	struct MMStdBase {

		static void mmPrint(const char* format, ...);
		static void mmAssert(char const* assertion, char const* file, unsigned int line);

	};

}