#pragma once

#include <utility>

namespace skylaunch {
	template<class R, class... Args>
	using FuncPtr = R (*)(Args...);

	template<class T, class R, class... Args>
	using MemberFuncPtr = R (T::*)(Args...);

	template<class T, class R, class... Args>
	inline auto FlattenMemberPtr(MemberFuncPtr<T, R, Args...> ptr) {
		// holy undefined behaviour batman
		union Hack {
			MemberFuncPtr<T, R, Args...> mPtr;
			FuncPtr<R, T*, Args...> cPtr;
		} u {
			.mPtr = ptr
		};

		return u.cPtr;
	}

	template<class T, class R, class... Args>
	using FlattenedMemberPtrType = decltype(FlattenMemberPtr(std::declval<MemberFuncPtr<T, R, Args...>>()));

} // namespace skylaunch