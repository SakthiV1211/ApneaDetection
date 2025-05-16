/*
 * SavitzkyGolayFilter.hpp
 *
 *  Created on: 13-Dec-2023
 *      Author: c11002
 */

#ifndef INC_SAVITZKYGOLAYFILTER_HPP_
#define INC_SAVITZKYGOLAYFILTER_HPP_

#include "SGCoeff.hpp"

template <std::size_t winSize>
double SavitzkyGolayFilter(const std::array<double, winSize> &inputData)
{
	double smoothData{0};
	for (uint16_t index = 0; index != winSize; index++)
	{
		smoothData += (inputData[index] * coeff[index]);
	}
	return smoothData;
}

#endif /* INC_SAVITZKYGOLAYFILTER_HPP_ */
