// Created by block on 5/28/23.

#include <xenomods/menu/Option.hpp>

#include "xenomods/Logger.hpp"

namespace xenomods {

	bool OptionBase::Update(HidInput* input) {
		if(input->InputUpStrict(Keybind::MENU_BACK)) {
			selected = false;
			return false;
		}

		return true;
	};

	void OptionBase::SetName(const std::string& name) {
		this->name = name;
	}

	void OptionBase::SetSelected() {
		selected = true;
	}

} // namespace xenomods