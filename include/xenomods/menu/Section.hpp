// Created by block on 5/28/23.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "xenomods/HidInput.hpp"

namespace xenomods {

	class Section {
	   protected:
		std::string key {};
		std::string display {};
		Section* parent {};

		std::vector<Section*> subsections {};
		std::vector<void(*)()> callbacks {};

	   public:
		Section(const std::string& key, const std::string& display);

		std::string GetName() {
			return display;
		}
		std::string GetKey() {
			return key;
		}
		Section* GetParent() {
			return parent;
		}
		std::vector<Section*>* GetSubsections() {
			return &subsections;
		}
		std::vector<void(*)()>* GetCallbacks() {
			return &callbacks;
		}

		void Render();

		Section* RegisterSection(const std::string& key, const std::string& display);
		void RegisterRenderCallback(void(*func)());
	};

} // namespace xenomods