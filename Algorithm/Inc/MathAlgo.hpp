#ifndef INC_ROUNDOFF_HPP_
#define INC_ROUNDOFF_HPP_

#include "cmath"
#include <stdint.h>
/**
 * @class mathAlgo
 * @brief The function shall check whether the input variable is greater than the comparator.
 */
class mathAlgo
{
  public:
	mathAlgo() = default;
	bool EpsilonCheck(const double, const double, const double relativeDiffFactor = 1e-5);
	double RoundUpMethod(const double, const uint8_t digit = 2U) const;
	double SimpsonIntegration(const double*, const uint16_t, double);
	double GetAverage(const double*, uint16_t);
	double GetStandardDeviation(const double*, uint16_t);
	template<typename Dtype>
	double GetAverageForPressLeak(const Dtype* inputPtr, uint16_t len);
};
//! @brief The function shall get average peak for leak.
//! @param const Dtype* inputPtr.
//! @param uint16_t len.
//! @retval -> avgCalc.
template<typename Dtype>
double mathAlgo::GetAverageForPressLeak(const Dtype* inputPtr, uint16_t len)
{
	double avgCalc{0.0};
	for(uint16_t i = 0U; i < len; i++)
	{
		avgCalc += inputPtr[i];
	}
	if(len != 0U)
	{
		avgCalc /= static_cast<double>(len);
	}
	return avgCalc;
}
#endif /* INC_COMMONDEFINE_HPP_ */
