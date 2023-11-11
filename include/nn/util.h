/**
* @file util.h
* @brief Helper functions for OS functionality.
*/

#pragma once

#include <type_traits>

#include "types.h"

namespace nn {
   namespace util {
	   struct Unorm8x4 {
		   u8 elements[0x4];
	   };

	   struct Color4u8 {
		   u8 r, g, b, a;
	   };

	   struct Float2 {
		   union {
			   float v[2];
			   struct {
				   float x;
				   float y;
			   };
		   };
	   };

	   struct Float3 {
		   union {
			   float v[3];
			   struct {
				   float x;
				   float y;
				   float z;
			   };
		   };
	   };

	   enum CharacterEncodingResult { Success,
									  BadLength,
									  InvalidFormat };

	   CharacterEncodingResult PickOutCharacterFromUtf8String(char*, char const** str);
	   CharacterEncodingResult ConvertCharacterUtf8ToUtf32(u32* dest, char const* src);
	   CharacterEncodingResult ConvertStringUtf16NativeToUtf8(char*, s32, u16 const*, s32);
	   CharacterEncodingResult ConvertStringUtf8ToUtf16Native(u16*, s32, char const*, s32);

	   class RelocationTable {
		  public:
		   void Relocate();
		   void Unrelocate();

		   s32 mMagic;		   // _0
		   u32 mPosition;	   // _4
		   s32 mSectionCount; // _8
	   };

	   class BinaryFileHeader {
		  public:
		   bool IsValid(s64 packedSig, s32 majorVer, s32 minorVer, s32 microVer) const;
		   bool IsRelocated() const;
		   bool IsEndianReverse() const;
		   nn::util::RelocationTable* GetRelocationTable();

		   s32 mMagic;				  // _0
		   u32 mSig;				  // _4
		   u8 mVerMicro;			  // _8
		   u8 mVerMinor;			  // _9
		   u16 mVerMajor;			  // _A
		   u16 mBOM;				  // _C
		   u8 mAlignment;			  // _E
		   u8 mTargetAddrSize;		  // _F
		   u32 mFileNameOffset;	  // _10
		   u16 mFlag;				  // _14
		   u16 mFirstBlockOffs;	  // _16
		   u32 mRelocationTableOffs; // _18
		   u32 mSize;				  // _1C
	   };

	   // not part of nn
	   namespace __detail {

		   // todo static assert.

		   template<s32 size>
		   using bit_size_holder = u8[size / 8];

		   template<s32 size>
		   struct best_type_for { using type = void; };

		   template<>
		   struct best_type_for<8> { using type = u8; };

		   template<>
		   struct best_type_for<16> { using type = u16; };

		   template<>
		   struct best_type_for<32> { using type = u32; };


		   template<s32 size>
		   using best_type_for_t = typename best_type_for<size>::type;
	   }

	   template<s32 size, typename T>
	   struct BitFlagSet {
		   using type = std::conditional_t<size <= 32, u32, u64>;
		   static const int storageBits = static_cast<int>(sizeof(type)) * 8;
		   static const int storageCount = static_cast<int>((size + storageBits - 1)) / storageBits;
		   type field[storageCount];

		   inline bool isBitSet(T index) const {
			   return (this->field[static_cast<u64>(index) / storageBits] &
					   (static_cast<type>(1) << static_cast<u64>(index) % storageBits)) != 0;
		   }
	   };

   }; // namespace util

   void ReferSymbol(void const*);
}; // namespace nn
