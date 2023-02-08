//
// Created by block on 7/21/2022.
//

#pragma once

#include <cstdint>

namespace Bdat {

	struct BdatSheet {
		char magic[4]; // should be BDAT
		ushort encryptionFlag;
		ushort nameOffset;
		ushort rowSize;
		ushort field4_0xa;
		ushort field5_0xc;
		ushort field6_0xe;
		ushort getIdCount;
		ushort idTop;
		ushort field9_0x14;
		ushort checksum;
		// there's more to it, but this is what's relevant atm
	};

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

	unsigned char* getMember(unsigned char* pBdat, const char* memberName);

	char* getMSLabel(unsigned char* pBdat, int index);
	char* getMSText(unsigned char* pBdat, int index);

	void* getVal(unsigned char* pBdat, unsigned char* pVarName, int);
	long getValCheck(unsigned char* pBdat, const char* memberName, int param_3, int type);

	uint16_t getIdCount(unsigned char* pBdat);
	uint16_t getIdTop(unsigned char* pBdat);
	char* getSheetName(unsigned char* pBdat);
	char* getMemberName(unsigned char* somethin, int index);

} // namespace Bdat