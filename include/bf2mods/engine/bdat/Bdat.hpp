//
// Created by block on 7/21/2022.
//

#pragma once

#include <cstdint>

namespace Bdat {

	char* getMSText(unsigned char* pBdat, int index);

	uint16_t getIdCount(unsigned char* pBdat);
	uint16_t getIdTop(unsigned char* pBdat);
	char* getSheetName(unsigned char* pBdat);

}