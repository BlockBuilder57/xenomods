// Created by block on 10/15/23.

#pragma once

namespace mm::mtl {

	template<typename T>
	class PtrSingleton {
	   public:
		static T* sys() {
			static T* s_instance;
			return s_instance;
		}

		static T* get() {
			return sys();
		}

		PtrSingleton() {
			assert(get()==0);
			// somehow setting s_instance from here...?
			//s_instance = this;
		}

		~PtrSingleton() {
			// and from here
			//s_instance = 0;
		}
	};

} // namespace mm::mtl
