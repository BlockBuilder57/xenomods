//
// Created by block on 1/8/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/mm/FixStr.hpp"

namespace ml {

	enum class FTYPE { /* file type? */
		kTypeInvalid = 0
	};

	struct [[gnu::packed]] FileHandleTh { /* PlaceHolder Structure */
		FTYPE mType;
		uint32_t field1_0x4;
		void * mMemBuffer;
		int mMaybeSizeButMaybeNot;
		uint32_t readStartOffset;
		int mCurrentReadOffset;
		uint32_t field9_0x1c;
		uint32_t field13_0x20;
		uint32_t readSize;
		uint32_t currentPtr;
		uint mAllocHandle;
		void* mUnkPtr;
		undefined field18_0x38;
		undefined field19_0x39;
		undefined field20_0x3a;
		undefined field21_0x3b;
		undefined field22_0x3c;
		undefined field23_0x3d;
		undefined field24_0x3e;
		undefined field25_0x3f;
		undefined field26_0x40;
		undefined field27_0x41;
		undefined field28_0x42;
		undefined field29_0x43;
		undefined field30_0x44;
		undefined field31_0x45;
		undefined field32_0x46;
		undefined field33_0x47;
		undefined field34_0x48;
		undefined field35_0x49;
		undefined field36_0x4a;
		undefined field37_0x4b;
		undefined field38_0x4c;
		undefined field39_0x4d;
		undefined field40_0x4e;
		undefined field41_0x4f;
		undefined field42_0x50;
		undefined field43_0x51;
		undefined field44_0x52;
		undefined field45_0x53;
		undefined field46_0x54;
		undefined field47_0x55;
		undefined field48_0x56;
		undefined field49_0x57;
		undefined8 field50_0x58;
		void * mWorkEvent;
		undefined8 field52_0x68;
		void * field53_0x70;
		undefined8 field54_0x78;
		undefined4 field55_0x80;
		undefined field56_0x84;
		undefined field57_0x85;
		undefined field58_0x86;
		undefined field59_0x87;
		undefined8 field60_0x88;
		undefined4 field61_0x90;
		mm::mtl::FixStr<256> filename;
		undefined field63_0x198;
		undefined field64_0x199;
		undefined field65_0x19a;
		undefined field66_0x19b;
		undefined field67_0x19c;
		undefined field68_0x19d;
		undefined field69_0x19e;
		undefined field70_0x19f;
		undefined field71_0x1a0;
		undefined field72_0x1a1;
		undefined field73_0x1a2;
		undefined field74_0x1a3;
		undefined field75_0x1a4;
		undefined field76_0x1a5;
		undefined field77_0x1a6;
		undefined field78_0x1a7;
		undefined field79_0x1a8;
		undefined field80_0x1a9;
		undefined field81_0x1aa;
		undefined field82_0x1ab;
		undefined field83_0x1ac;
		undefined field84_0x1ad;
		undefined field85_0x1ae;
		undefined field86_0x1af;
		undefined field87_0x1b0;
		undefined field88_0x1b1;
		undefined field89_0x1b2;
		undefined field90_0x1b3;
		undefined field91_0x1b4;
		undefined field92_0x1b5;
		undefined field93_0x1b6;
		undefined field94_0x1b7;
		undefined4 field95_0x1b8;
		undefined field96_0x1bc;
		undefined field97_0x1bd;
		undefined field98_0x1be;
		undefined field99_0x1bf;
		undefined8 time;
		undefined field101_0x1c8;
		undefined field102_0x1c9;
		undefined field103_0x1ca;
		undefined field104_0x1cb;
		undefined field105_0x1cc;
		undefined field106_0x1cd;
		undefined field107_0x1ce;
		undefined field108_0x1cf;
		uint mAlignmentMaybe;
		undefined field110_0x1d4;
		undefined field111_0x1d5;
		undefined field112_0x1d6;
		undefined field113_0x1d7;
		undefined4 field114_0x1d8;
		undefined field115_0x1dc;
		undefined field116_0x1dd;
		undefined field117_0x1de;
		undefined field118_0x1df;
		undefined8 field119_0x1e0;
		undefined4 field120_0x1e8;
		undefined8 field121_0x1ec;
		undefined1 mValidMaybe; /* Created by retype action */
		byte mFlags;
		undefined field124_0x1f6;
		undefined field125_0x1f7;
		undefined field126_0x1f8;
		undefined4 field127_0x1f9;
		uint mCancelledMaybe;
		undefined4 field129_0x201;
		undefined field130_0x205;
	};

	struct FileReadResult { /* PlaceHolder Structure */
		ulong bytesRead;
		bool bFullyLoaded;
		undefined field2_0x9;
		undefined field3_0xa;
		undefined field4_0xb;
	};

	struct DevFileParamTh { /* PlaceHolder Structure */
		mm::mtl::FixStr<256>* RequestedFilename;
		int field1_0x8;
		int field2_0xc;
		void* field3_0x10;
		INSERT_PADDING_BYTES(31);
		long field36_0x38;
		void* readBuffer;
		uint32_t readSize;
		uint32_t readStartOffset;
		uint32_t field40_0x50;
		uint32_t field41_0x54;
		uint32_t field42_0x58;
		uint32_t field43_0x5c;
		void* userData;
		int field45_0x68;
		uint8_t field46_0x6c;
		uint8_t field47_0x6d;
		uint16_t field48_0x6e;
	};

	enum class MEDIA : std::uint8_t {
		Default,      // supposedly covers everything else (like rom:)
		unknown,      // this *is* a type, but not sure which one
		DeviceRoot,   // usbkey_ and //
		Host,         // host:
		unknown2,     // unused?
		USB,          // usb:
		NandAndSave,  // nand: and save:
		AddOnContent, // aoc(0-9):
		SDCard        // sd:
	};

	struct DevFileTh {
		static void initializePath(const char* mount, bool isArchiveMaybe);
		static void registArchive(MEDIA media, const char* headerPath, const char* dataPath, const char* mount);
		static void addMediaPath(MEDIA media, const char* mount, int unknown = 1);

		static bool checkValidFileBlock(const char* path);
	};

} // namespace ml