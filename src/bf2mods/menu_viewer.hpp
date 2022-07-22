#pragma once

#include <cstdint>

namespace bf2mods::MenuViewer {

	extern void* UIManagerPtr;

	void OpenLayer(unsigned int layer);
	void CloseLayer(unsigned int layer);

	void Setup();

} // namespace bf2mods::MenuViewer
