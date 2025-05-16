#ifndef INC_WINDOWFUNCTION_HPP
#define INC_WINDOWFUNCTION_HPP

#include <array>
#include <stdint.h>
class windowFunction
{
  public:
	template<typename winDataType, std::size_t lenOfFFT>
	void HanningWindow(std::array<winDataType, lenOfFFT>&, const uint16_t);
};
//! @brief
//! @param
//! @retval

template<typename winDataType, std::size_t lenOfFFT>
void windowFunction::HanningWindow(std::array<winDataType, lenOfFFT>& argWindowedSignal,
								   const uint16_t hanningWinLen)
{
	for(uint16_t seg = 0U; seg < hanningWinLen; seg++)
	{
		const double windowValue{0.5 * (1U - (cos((2.0 * PI * static_cast<double>(seg)) /
												  static_cast<double>(hanningWinLen - 1U))))};
		argWindowedSignal.at(seg) = argWindowedSignal.at(seg) * windowValue;
	}
}

#endif /*INC_WINDOWFUNCTION_HPP*/