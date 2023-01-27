//
// Created by block on 1/22/2023.
//

#pragma once

namespace ui {

	enum UIStrType : int {
		kInvalid = 0,
		kFromBdat = 1,
		kFromString = 2
	};

	struct UIStr {
	   public:
		char* text;
		int unk1;
		int unk2;
		UIStrType type;
		int unk3;

		UIStr();
		UIStr(const char* text, bool doAllocate);

		char* c_str();
	};

}