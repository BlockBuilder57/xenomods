// Created by block on 6/17/23.

#pragma once

#include <xenomods/Utils.hpp>

namespace mm {

	namespace mtl {

		template<typename T, std::size_t N>
		class [[gnu::packed]] FixedVector {
		   public:
			T items[N] {};
			std::uint64_t m_nLen {};

			void set(std::vector<T> vec) {
				std::size_t min = std::min(vec.size(), N);

				for(size_t i; i < min; i++) {
					items[i] = vec[i];
				}

				m_nLen = min;
			}

			std::vector<T> getVec() {
				return std::vector<T>(&items[0], &items[N]);
			}
		};

	} // namespace mtl

} // namespace mm
