// Created by block on 8/26/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_OLD_ENGINE
namespace ptlib::emitter {

	class EmitterObj {
	   public:
		INSERT_PADDING_BYTES(368)
		int category;
		INSERT_PADDING_BYTES(250)
		std::uint8_t typeOrFlags;

		bool isRenderCheck() const;
	};

}; // namespace ptlib::emitter
#endif