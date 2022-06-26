#include <bf2mods/mm/math_types.hpp>

namespace mm {

	const Vec3 Vec3::zero { 0.f, 0.f, 0.f };
	const Quat Quat::zero { 0.f, 0.f, 0.f, 0.f };

	const Mat44 Mat44::identity { 1.f, 0.f, 0.f, 0.f,
								  0.f, 1.f, 0.f, 0.f,
								  0.f, 0.f, 1.f, 0.f,
								  0.f, 0.f, 0.f, 1.f };

	const Col4 Col4::White { 1.f, 1.f, 1.f, 1.f };
	const Col4 Col4::Black { 0.f, 0.f, 0.f, 1.f };

} // namespace mm