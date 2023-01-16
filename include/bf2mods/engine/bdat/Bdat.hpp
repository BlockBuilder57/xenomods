//
// Created by block on 7/21/2022.
//

#pragma once

#include <cstdint>

namespace Bdat {

	// real name unknown for now
	enum ValueType : int {
		kNone = 0,
		kUByte = 1,
		kUInt16 = 2,
		kUInt32 = 3,
		kSByte = 4,
		kInt16 = 5,
		kInt32 = 6,
		kString = 7,
		kFloat = 8
	};

	char* getMSText(unsigned char* pBdat, int index);

	long getValCheck(unsigned char* pBdat, const char* columnName, int param_3, int type);

	uint16_t getIdCount(unsigned char* pBdat);
	uint16_t getIdTop(unsigned char* pBdat);
	char* getSheetName(unsigned char* pBdat);

}