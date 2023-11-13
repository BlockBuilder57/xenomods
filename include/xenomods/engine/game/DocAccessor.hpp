// Created by block on 6/18/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "Controllers.hpp"
#include "Data.hpp"
#include "Managers.hpp"
#include "Sequence.hpp"
#include "xenomods/engine/fw/Document.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class DocAccessor {
	   public:
		DataManager* getDataManager() const;
		void* getPartyManager();
		SeqManager* getSeqManager() const;

		// NOT A REAL FUNCTION
		static inline DocAccessor* GetFromXenomodsDocument() {
			return reinterpret_cast<game::DocAccessor*>(&xenomods::DocumentPtr);
		}
	};

}
#endif