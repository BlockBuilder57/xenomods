// Created by block on 5/28/23.

#pragma once

#include <string>
#include <vector>
#include <xenomods/engine/mm/MathTypes.hpp>

#include "Option.hpp"
//#include "Menu.hpp"

namespace xenomods {

	class Section {
	   protected:
		std::string key;
		std::string display;
		Section* parent;

		struct Textual {
			std::string text;
			mm::Col4 color;
		};

		std::vector<Section> subsections;
		std::vector<Option> options;
		std::vector<Textual> textuals;

	   public:
		Section(const std::string& key, const std::string& display);

		int GetMaxIndex() {
			return subsections.size() + options.size();
		}
		std::string GetName() {
			return display;
		}
		Section* GetParent() {
			return parent;
		}

		void Update();
		void PerformSelect(int index);

		void Render(mm::Pnt<int>& pnt);

		void AddTextual(const std::string& text, const mm::Col4& color);
		void AddTextual(const std::string& text) { AddTextual(text, {}); };
	};

} // namespace xenomods