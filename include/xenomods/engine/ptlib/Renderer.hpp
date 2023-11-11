// Created by block on 8/26/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_OLD_ENGINE
namespace ptlib::renderer {

	namespace sort {
		class EmitterSort {
		};
	} // namespace sort

	class RenderGrObj {
	   public:
		void renderEmitters(const mm::mtl::FixedVector<sort::EmitterSort, 4000>& emitters, int maybeType);
		void renderEmitter(emitter::EmitterObj* emitter, int& param_2, bool param_3);
		void draw_capture();
	};

} // namespace ptlib::renderer
#endif