// Created by block on 5/28/23.

#include <xenomods/menu/Option.hpp>

#include "xenomods/Logger.hpp"

namespace xenomods {

	bool Option<float>::Update(HidInput* input) {
		if(!OptionBase::Update(input))
			return false;

		float& val = ValueAs<float>();

		tens = std::log10(std::abs(val));
		by2 = input->InputHeld(Keybind::MENU_NUM_MULT2);
		by10s = input->InputHeld(Keybind::MENU_NUM_TENS);

		if (tens <= 0)
			tens = 1;

		if(input->InputDown(Keybind::MENU_NUM_INC)) {
			if(by2)
				val *= 2;
			else if(by10s && tens != 0)
				val += std::pow(10, tens);
			else
				val++;
		} else if(input->InputDown(Keybind::MENU_NUM_DEC)) {
			if(by2)
				val /= 2;
			else if(by10s && tens != 0)
				val -= std::pow(10, tens);
			else
				val--;
		}

		if (input->InputDown(Keybind::MENU_NUM_NEGATE)) {
			val = -val;
		}

		return true;
	}

	std::string Option<float>::String() const {
		std::string extra;

		if (selected) {
			if (!by2 && !by10s) {
				extra = " \x81\x7D""1"; // +- 1
			}
			else if (by2) {
				extra = " \x81\x7E/\x81\x80""2"; // x/div 2
			}
			else if (by10s) {
				// 0x7D is }, so it gets mad when trying to emit it on its own
				extra = fmt::format(" {}{}", "\x81\x7D", std::pow(10, tens)); // +- 10^tens
			}
		}

		return fmt::format("{}: {}{}", name, OptionBase::ValueAs<float>(), extra);
	}

} // namespace xenomods