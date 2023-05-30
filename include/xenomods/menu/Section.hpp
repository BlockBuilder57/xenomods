// Created by block on 5/28/23.

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <xenomods/engine/mm/MathTypes.hpp>

#include "Option.hpp"
//#include "Menu.hpp"

namespace xenomods {

	class Section {
	   protected:
		std::string key {};
		std::string display {};
		Section* parent {};

		struct Textual {
			std::string text {};
			mm::Col4 color {};

			Textual(std::string text, mm::Col4 color) : text(std::move(text)), color(color) {};
		};

		std::vector<Section> subsections {};
		std::vector<OptionBase*> options {};
		std::vector<Textual> textuals {};

		OptionBase* curOption {};

	   public:
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
			return subsections.size() + options.size();
		}
		bool IsSelectingOption() {
			return curOption != nullptr;
		}
		std::vector<Section>* GetSubsections() {
			return &subsections;
		}

		void Update(HidInput* input);
		void PerformSelect();

		void Render(mm::Pnt<int>& pnt);


		Section* RegisterSection(const std::string& key, const std::string& display);

		template<class T, class ...Args>
		void RegisterOption(Args&&... args) {
			options.push_back(new Option<T>(static_cast<Args&&>(args)...));
		}

		void RegisterOption(OptionBase* opt) {
			options.push_back(opt);
		}

		void RegisterTextual(const std::string& text, const mm::Col4& color);
		void RegisterTextual(const std::string& text) { RegisterTextual(text, {}); };
	};

} // namespace xenomods