//
// Created by block on 1/23/2023.
//

#pragma once

namespace ml {

	/*
	 * mt = Mersenne twister, a popular PRNG
	 * with a big ol period length
	 */

	void mtInit(unsigned int seed); // default seed is 0x12bd6aa

	int mtRand();
	int mtRand(int max);
	int mtRand(int min, int max);

	float mtRandf1();
	float mtRandf1(float max);
	float mtRandf1(float min, float max);

	float mtRandf2();
	float mtRandf2(float max);
	float mtRandf2(float min, float max);

	float mtRandf3();
	float mtRandf3(float max);
	float mtRandf3(float min, float max);

} // namespace ml