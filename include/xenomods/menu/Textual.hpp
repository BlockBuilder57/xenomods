// Created by block on 6/9/23.

#pragma once

#include <string>
#include <xenomods/engine/mm/MathTypes.hpp>

namespace xenomods {

	struct Textual {
	   public:
		Textual(std::string text, mm::Col4 color, std::string (*callback)())
			: text(std::move(text)), color(color), callback(callback) {};

		Textual(std::string text, mm::Col4 color)
			: Textual(text, color, nullptr) {};

		std::string String() {
			if(callback != nullptr)
				return text + callback();

			return text;
		};

		mm::Col4 Color() {
			return color;
		};

	   private:
		std::string text {};
		mm::Col4 color {};
		std::string (*callback)() {};
	};

}