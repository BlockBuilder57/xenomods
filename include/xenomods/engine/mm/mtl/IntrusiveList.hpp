// Created by block on 3/14/2023.

#pragma once

#include <xenomods/Utils.hpp>

namespace mm::mtl {

	struct IntrusiveListNodeBase {
		IntrusiveListNodeBase* next;
		IntrusiveListNodeBase* prev;
	};

	struct IntrusiveListBase {
		IntrusiveListNodeBase* head;
		IntrusiveListNodeBase* tail;
		std::size_t count;
	};

}