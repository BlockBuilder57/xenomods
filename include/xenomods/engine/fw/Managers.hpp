// Created by block on 5/25/23.

#pragma once

#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/fw/Document.hpp"

namespace fw {

#if XENOMODS_NEW_ENGINE
	class SceneManager {
	   public:
		void* vtable;
		ml::Scn* scene;

		void initialize(fw::Document& doc);
	};
#endif

}