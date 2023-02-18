//
// Created by block on 1/28/2023.
//

#pragma once

namespace mm::mtl {

	struct RTTI {
		const char* szName;

		RTTI* pPrevBase;
		void* unk;

		bool isKindOf(const RTTI* pBase) const;
	};

}