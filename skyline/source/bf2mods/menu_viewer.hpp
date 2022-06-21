#pragma once

#include <cstdint>

namespace bf2mods {

	extern void* UIManagerPtr;

	void OpenLayer(unsigned int layer);
	void CloseLayer(unsigned int layer);

	void SetupMenuViewer();

} // namespace bf2mods
