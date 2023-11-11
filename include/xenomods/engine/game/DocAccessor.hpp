// Created by block on 6/18/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "Controllers.hpp"
#include "Sequence.hpp"
#include "xenomods/engine/fw/Document.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class DocAccessor {
	   public:
		void* getDataManager();
		void* getPartyManager();
		SeqManager* getSeqManager() const;
	};

}
#endif