// Created by block on 5/28/23.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Option.hpp"
#include "Textual.hpp"
//#include "Menu.hpp" // cyclical reference :(

namespace xenomods {

	class Section {
	   protected:
		std::string key {};
		std::string display {};
		Section* parent {};

		std::vector<Section*> subsections {};
		std::vector<OptionBase*> options {};
		std::vector<Textual> textuals {};

		OptionBase* curOption {};

	   public:
		int SavedIndex {};

		Section(const std::string& key, const std::string& display);
		// for some reason, leaving the dtor in crashes. ???
		/*~Section() {
			for(auto* option : options) {
				delete option;
			}
		}*/

		std::string GetName() {
			return display;
		}
		std::string GetKey() {
			return key;
		}
		Section* GetParent() {
			return parent;
		}
		int GetMaxIndex() {
			return subsections.size() + options.size() - 1;
		}
		bool IsSelectingOption() {
			return curOption != nullptr;
		}
		std::vector<Section*>* GetSubsections() {
			return &subsections;
		}

		void Update(HidInput* input);
		void PerformSelect();

		void Render(mm::Pnt<int>& pnt);

		Section* RegisterSection(const std::string& key, const std::string& display);

		template<class T, class... Args>
		OptionBase* RegisterOption(Args&&... args) {
			return options.emplace_back(new Option<T>(static_cast<Args&&>(args)...));
		}

		void RegisterOption(OptionBase* opt) {
			options.push_back(opt);
		}

		void RegisterTextual(const std::string& text, const mm::Col4& color, std::string (*callback)());
		void RegisterTextual(const std::string& text, const mm::Col4& color) {
			RegisterTextual(text, color, nullptr);
		};
		void RegisterTextual(const std::string& text) {
			RegisterTextual(text, {}, nullptr);
		};
	};

} // namespace xenomods