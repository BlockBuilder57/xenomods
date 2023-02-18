#include <xenomods/stuff/utils/util.hpp>

namespace util::detail {

	void nnRandBase(void* input, std::size_t size) {
		nn::os::GenerateRandomBytes(input, size);
	}

} // namespace util::detail
