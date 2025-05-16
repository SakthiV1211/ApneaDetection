/**
The function shall check whether the input variable is greater than the comparator using
 */
#include "MathAlgo.hpp"

//! @brief The function shall check whether the input variable is greater than the comparator using
//! precision methods.
//! @param inputDataArg-> Input data.
//! @param compareVariableArg-> Data to be compared.
//! @param relativeDiffFactor-> Cut off difference between two values.
//! @retval true -> greater, false -> lower.

bool mathAlgo::EpsilonCheck(const double inputDataArg, const double compareVariableArg,
							const double relativeDiffFactor)
{
	if(inputDataArg > compareVariableArg)
	{
		return true;
	}
	else
	{
		if(std::abs(compareVariableArg - inputDataArg) <= relativeDiffFactor)
		{
			return true;
		}
	}
	return false;
}

//! @brief The function shall round up the data to n decimal places.
//! @param varInput -> Input data.
//! @param digit -> Decimal places.
//! @retval -> round up value.

double mathAlgo::RoundUpMethod(const double varInput, const uint8_t digit) const
{
	int32_t round{static_cast<int32_t>(varInput * pow(10U, static_cast<uint32_t>(digit) + 1U))};
	if((round % static_cast<int32_t>(10U)) >= static_cast<int32_t>(5U))
	{
		round += static_cast<int32_t>(10U);
	}
	round /= static_cast<int32_t>(10U);
	const double powerDen{static_cast<double>(pow(10U, digit))};
	double roundOffReturn{static_cast<double>(round) / powerDen};
	return roundOffReturn;
}

//! @brief The function shall perform simpson's 1/3 rule formula. Simpson’s 1/3rd rule is an
//! extension of the trapezoidal rule in which the integrand is approximated by a second-order
//! polynomial.
//! @param yOfX -> pointer which contains input data address.
//! @param lenOfData -> Length of the input data.
//! @param dx -> Width of each subintreval.
//! @retval Integrated value.

double mathAlgo::SimpsonIntegration(const double* yOfX, const uint16_t lenOfData, double dx)
{
	double mSipms{static_cast<double>(yOfX[0U] + yOfX[lenOfData - 1U])};
	const uint16_t totalLen{static_cast<uint16_t>(static_cast<uint32_t>(lenOfData) - 1U)};
	for(uint16_t index = 1U; index < totalLen; index++)
	{
		if((index % 2U) == 0U)
		{
			mSipms += (2U * yOfX[index]);
		}
		else
		{
			mSipms += (4U * yOfX[index]);
		}
	}
	mSipms = mSipms * (dx / 3U);
	return mSipms;
}

//! @brief The function shall used to calculate the average formula for the given input.
//! @param inputPtr-> Input array address.
//! @param len-> Length of the input data to perform average.
//! @retval avgCalc-> average of the input.

double mathAlgo::GetAverage(const double* inputPtr, uint16_t len)
{
	if(len == 0)
	{
		return 0.0;
	}
	double avgCalc{0.0};
	for(uint16_t i = 0U; i < len; i++)
	{
		avgCalc += inputPtr[i];
	}
	avgCalc /= static_cast<double>(len);
	return avgCalc;
}

//! @brief The function shall used to calculate the statndard deviation formula for the given input.
//! @param inputPtr-> Input array address.
//! @param len-> Length of the input data to perform standard deviation.
//! @retval avgCalc-> standard deviation value of the input.

double mathAlgo::GetStandardDeviation(const double* inputPtr, uint16_t len)
{
	if(len == 0)
	{
		return 0.0;
	}
	double getMeanInput{GetAverage(inputPtr, len)};
	double variance{0.0};
	for(uint16_t i = 0U; i < len; i++)
	{
		variance += pow((inputPtr[i] - getMeanInput), 2.0);
	}
	variance /= static_cast<double>(len - 1);
	double sdData{sqrt(variance)};
	return sdData;
}
