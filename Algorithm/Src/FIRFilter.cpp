/**
 * VTITAN CONFIDENTIAL
 * Copyright © 2014 – 2019 vTitan Corporation.
 * All Rights Reserved
 *
 * Confidential Information. The information contained herein is, and remains
 * the property of vTitan Corporation Private Ltd, Chennai, India and vTitan
 * Corporation, Pleasanton, CA USA and its suppliers, if any. The information
 * contained here shall not be copied, used, disclosed, or reproduced except as
 * specifically authorized by vTitan Corporation.
 */
#include "FIRFilter.hpp"

FIRFilter::FIRFilter() : xOfNCircularBuffer{0}, filteredYofN{0}, inputIndex{0}
{
}
//! @brief This function shall process the convolution of input data and impulse response.
//! @param None
//! @return None

void FIRFilter::ConvolutionFunction(void)
{
	filteredYofN = 0;
	for(uint16_t impulseIndex = 0; impulseIndex < LENGTH_OF_IMPULSE_RESPONSE; impulseIndex++)
	{
		filteredYofN =
			filteredYofN + (xOfNCircularBuffer[inputIndex] * impulseRespone[impulseIndex]);
		inputIndex--;
	}
}
//! @brief The function shall perform FIR filter operation.
//! @param xOfN -> Input raw data.
//! @retval double -> filtered data

double FIRFilter::FilterEquation(double xOfN)
{
	inputIndex = LENGTH_OF_IMPULSE_RESPONSE - 1U;

	(void)std::copy(xOfNCircularBuffer.begin() + 1, xOfNCircularBuffer.end(),
					xOfNCircularBuffer.begin());
	xOfNCircularBuffer.at(inputIndex) = xOfN;

	ConvolutionFunction();
	return filteredYofN;
}
