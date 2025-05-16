#ifndef INC_FIRFILTER_HPP_
#define INC_FIRFILTER_HPP_

#include <array>
#include <stdint.h>
#include "FilterCoeffDefine.hpp"
using namespace filerCoeffDefine;
/**
 * @class FIRFilter
 * @brief The class FIR filter.
 */
class FIRFilter
{
  private:
	std::array<double, LENGTH_OF_IMPULSE_RESPONSE> xOfNCircularBuffer;
	double filteredYofN;
	uint16_t inputIndex;

	void ConvolutionFunction(void);

  public:
	double FilterEquation(double);
	FIRFilter();
};

#endif /* INC_IIRFILTER_HPP_ */
