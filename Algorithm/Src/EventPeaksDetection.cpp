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

#include "EventPeaksDetection.hpp"
uint32_t peakIndexBuffer{0U};

//! @brief The function shall reset all eventPeaksDetection data members.
//! @param None.
//! @retval None.
eventPeaksDetection::eventPeaksDetection() : extendSecs{extendedSecsState::zero_sec}, getRecWave{waveform::sine_wave}, negPeakCount{0U},
											 positionIndex{0U}, peakIndexPosition{0U}, intervalGapAdjCounter{0U},
											 maxSlopeCounter{static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE * 3U))},
											 slopeFactorConst{4}, peakBuffer{0.0}, peakSlopeDiff{0.0},
											 eventPeakDetectVar{detectState::peak_detect, detectionAlgorithmState::wait_for_500ms, 25U},
											 basicDetectionHandler{}, reconfirmDetectionHandler{}, flowRateFrame{}, peakDataInSec{}
{
	basicDetectionHandler.at(static_cast<std::size_t>(0U)) =
		&eventPeaksDetection::PositivePeakBasicCheck;
	basicDetectionHandler.at(static_cast<std::size_t>(1U)) =
		&eventPeaksDetection::NegativePeakBasicCheck;

	reconfirmDetectionHandler.at(static_cast<std::size_t>(0U)) =
		&eventPeaksDetection::PositivePeakReconfirmCheck;
	reconfirmDetectionHandler.at(static_cast<std::size_t>(1U)) =
		&eventPeaksDetection::NegativePeakReconfirmCheck;
}

//! @brief The function shall detect positive peak data from the input data array.
//! @param None
//! @retval True/False -> Returns true if peak detected or else it returns false.

bool eventPeaksDetection::PositivePeakBasicCheck(void)
{
	if ((apneaDetectionObj.mathAlgoObj.EpsilonCheck(flowRateFrame.dataFrame.at(1U),
													flowRateFrame.dataFrame.at(0U))) &&
		(apneaDetectionObj.mathAlgoObj.EpsilonCheck(flowRateFrame.dataFrame.at(1U),
													flowRateFrame.dataFrame.at(2U))))
	{
		return true;
	}
	return false;
}

//! @brief The function shall detect valley from the input data array.
//! @param None
//! @retval True/False -> Returns true if peak detected or else it returns false.

bool eventPeaksDetection::NegativePeakBasicCheck(void)
{
	if ((apneaDetectionObj.mathAlgoObj.EpsilonCheck(flowRateFrame.dataFrame.at(0U),
													flowRateFrame.dataFrame.at(1U))) &&
		(apneaDetectionObj.mathAlgoObj.EpsilonCheck(flowRateFrame.dataFrame.at(2U),
													flowRateFrame.dataFrame.at(1U))))
	{
		return true;
	}
	return false;
}

//! @brief The function shall reset all PositivePeakReconfirmCheck data members.
//! @param None.
//! @retval none.

void eventPeaksDetection::PositivePeakReconfirmCheck(void)
{
	if (peakSlopeDiff <= -(slopeFactorConst * 1.5))
	{
		CheckForPrevSecs();
		eventPeakDetectVar.goToNextDetectState = detectState::valley_detect;
		maxSlopeCounter = static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE));
		eventPeakDetectVar.peakToValleyIntrevalGap = intervalGapAdjCounter;
		intervalGapAdjCounter = 0u;
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::wait_for_500ms;
	}
	else if (peakSlopeDiff >= (slopeFactorConst * 0.5))
	{
		if (extendSecs != extendedSecsState::zero_sec)
		{
			extendSecs = extendedSecsState::zero_sec;
		}
		intervalGapAdjCounter = 0u;
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::find_peak_algo;
	}
	else
	{
	}
}
//! @brief The function shall reset all NegativePeakReconfirmCheck data members.
//! @param None.
//! @retval none.

void eventPeaksDetection::NegativePeakReconfirmCheck(void)
{
	if (peakSlopeDiff >= slopeFactorConst)
	{
		valleyFrame.AppendData(peakBuffer);
		eventPeakDetectVar.goToNextDetectState = detectState::peak_detect;
		maxSlopeCounter = static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE * 3U));
		eventPeakDetectVar.peakToValleyIntrevalGap = intervalGapAdjCounter;
		intervalGapAdjCounter = 0u;
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::wait_for_500ms;
	}
	else if (peakSlopeDiff <= -(slopeFactorConst * 0.5))
	{
		intervalGapAdjCounter = 0u;
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::find_peak_algo;
	}
	else
	{
	}
}

//! @brief The function shall fill the data packet if the algorithm is in finding peak/valley. Once
//! the peak is detected the function shall wait around 500ms for the next valley detection and
//! vice-versa.
//! @param filteredFlowData -> Input data.
//! @retval None.

void eventPeaksDetection::FlowRatePeakDetectionMethod(const double filteredFlowData)
{
	++peakIndexTest;
	++positionIndex;
	flowRateFrame.CircularDataAppend(filteredFlowData);
	switch (eventPeakDetectVar.algorithmState)
	{
	case detectionAlgorithmState::find_peak_algo:
		if ((this->*basicDetectionHandler.at(static_cast<std::size_t>(
						eventPeakDetectVar.goToNextDetectState)))() == true)
		{
			peakBuffer = flowRateFrame.dataFrame.at(1U);
			peakIndexPosition = static_cast<uint16_t>(static_cast<uint32_t>(positionIndex) -
													  static_cast<uint32_t>(1U));
			peakIndexBuffer = peakIndexTest;
			eventPeakDetectVar.algorithmState = detectionAlgorithmState::peak_confirm_algo;
		}
		break;

	case detectionAlgorithmState::peak_confirm_algo:
		if (ConfirmationCheckTime() == true)
		{
			ReConfirmPeak();
		}
		break;

	case detectionAlgorithmState::wait_for_500ms:
		EventPeaksIntrevalGap();
		break;
	default:
		break;
	}
}

//! @brief The function shall reset all eventPeaksDetection data members.
//! @param None.
//! @retval None.

void eventPeaksDetection::ReConfirmPeak(void)
{
	peakSlopeDiff = (flowRateFrame.dataFrame.at(2U) - peakBuffer);
	(this->*reconfirmDetectionHandler.at(
				static_cast<std::size_t>(eventPeakDetectVar.goToNextDetectState)))();
}

//! @brief The function shall reset all eventPeaksDetection data members.
//! @param None.
//! @retval None.

bool eventPeaksDetection::ConfirmationCheckTime(void)
{
	++intervalGapAdjCounter;
	if (intervalGapAdjCounter >= maxSlopeCounter)
	{
		if (eventPeakDetectVar.goToNextDetectState == detectState::peak_detect)
		{
			CheckForPrevSecs();
		}
		eventPeakDetectVar.goToNextDetectState = detectState::peak_detect;
		maxSlopeCounter = static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE * 3U));
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::find_peak_algo;
		intervalGapAdjCounter = 0;
		return false;
	}
	return true;
}

//! @brief The function shall wait around 500ms for the interval gap between peak and valley or
//! vice-versa.
//! @param None
//! @retval None

void eventPeaksDetection::EventPeaksIntrevalGap(void)
{
	++eventPeakDetectVar.peakToValleyIntrevalGap;
	if (eventPeakDetectVar.peakToValleyIntrevalGap >= static_cast<uint16_t>(SAMPLING_RATE / 2U))
	{
		eventPeakDetectVar.algorithmState = detectionAlgorithmState::find_peak_algo;
		eventPeakDetectVar.peakToValleyIntrevalGap = 0U;
	}
}

//! @brief The function shall reset all eventPeaksDetection data members.
//! @param None.
//! @retval None.

void eventPeaksDetection::ResetEventPeakInfoData(void)
{
	extendSecs = extendedSecsState::zero_sec;
	negPeakCount = 0U;
	eventPeakDetectVar.peakToValleyIntrevalGap = 0U;
	eventPeakDetectVar.algorithmState = detectionAlgorithmState::wait_for_500ms;
	eventPeakDetectVar.goToNextDetectState = detectState::peak_detect;
	maxSlopeCounter = static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE * 3U));

	flowRateFrame.ResetFrameSeg();
	peakFrame.ResetFrameSeg();
	peakIndexFrame.ResetFrameSeg();
	valleyFrame.ResetFrameSeg();

	apneaDetectionObj.ResetAhiValues();
}

//! @brief The function shall reset all eventPeaksDetection data members.
//! @param argReceivedWave -> .
//! @retval None.

void eventPeaksDetection::ProcessPeakData(const waveform argReceivedWave)
{
	getRecWave = argReceivedWave;
	GetPeaksInfoForEverySec();
	peakFrame.frameFull = true;
	peakIndexFrame.frameFull = true;
	valleyFrame.frameFull = true;
	positionIndex = 0;
}


//! @brief The function is designed to extract peak data from the frame packet for every second and
//! subsequently transfer this data for AHI detection.
//! @param None.
//! @retval None.

void eventPeaksDetection::GetPeaksInfoForEverySec(void)
{
	ResetDataFrame();
	const uint16_t totFrameIndex{static_cast<uint16_t>(peakIndexFrame.frameIndex)};
	for (uint16_t readIndex = 0; readIndex < totFrameIndex; readIndex++)
	{
		uint16_t getPosInSec{static_cast<uint16_t>(peakIndexFrame.dataFrame.at(readIndex) / SAMPLING_RATE)};
		peakDataInSec.at(getPosInSec) = std::make_pair(1, peakFrame.dataFrame.at(readIndex));
	}
	TransferPeakValidation();
}

//! @brief This function transfers data related to the last packet's peak detection. Specifically,
//! if the peak detection state indicates a positive peak confirmation, the function will transfer
//! the peak data up to, but not including the confirmation second.
//! @param None.
//! @retval None.

void eventPeaksDetection::TransferPeakValidation(void)
{
	if ((eventPeakDetectVar.algorithmState == detectionAlgorithmState::peak_confirm_algo) &&
		(eventPeakDetectVar.goToNextDetectState == detectState::peak_detect))
	{
		if (intervalGapAdjCounter > (SAMPLING_RATE * 2U))
		{
			extendSecs = extendedSecsState::three_secs;
			TransferPeakForPcDetection(7U);
		}
		else if (intervalGapAdjCounter > SAMPLING_RATE)
		{
			extendSecs = extendedSecsState::two_secs;
			TransferPeakForPcDetection(8U);
		}
		else
		{
			extendSecs = extendedSecsState::one_sec;
			TransferPeakForPcDetection(9U);
		}
	}
	else
	{
		extendSecs = extendedSecsState::zero_sec;
		TransferPeakForPcDetection(10U);
	}
}

//! @brief The function is designed to handle peak data based on the last packet's peak detection.
//! If the end state is zero, the function will store the peak data in the current frame packet.
//! Conversely, if the end state is non-zero, the function will transmit the peak data information
//! from the last confirmed seconds to the event detection process.

//! @param None.
//! @retval None.

void eventPeaksDetection::CheckForPrevSecs(void)
{
	switch (extendSecs)
	{
	case extendedSecsState::zero_sec:
		peakFrame.AppendData(peakBuffer);
		peakIndexFrame.AppendData(peakIndexPosition);
		peakPosPrint << (peakIndexBuffer - 1U) << ", " << peakBuffer << std::endl;
		break;

	case extendedSecsState::one_sec:
		apneaDetectionObj.FindOutAH(std::make_pair(1, peakBuffer), getRecWave);
		peakPosPrint << (peakIndexBuffer - 1U) << ", " << peakBuffer << std::endl;
		break;

	case extendedSecsState::two_secs:
		peakPosPrint << (peakIndexBuffer - 1U) << ", " << peakBuffer << std::endl;
		apneaDetectionObj.FindOutAH(std::make_pair(1, peakBuffer), getRecWave);
		apneaDetectionObj.FindOutAH(std::make_pair(0, 0.0), getRecWave);
		break;

	case extendedSecsState::three_secs:
		peakPosPrint << (peakIndexBuffer - 1U) << ", " << peakBuffer << std::endl;
		apneaDetectionObj.FindOutAH(std::make_pair(1, peakBuffer), getRecWave);
		apneaDetectionObj.FindOutAH(std::make_pair(0, 0.0), getRecWave);
		apneaDetectionObj.FindOutAH(std::make_pair(0, 0.0), getRecWave);
		break;

	default:
		break;
	}
	extendSecs = extendedSecsState::zero_sec;
}

//! @brief The function shall used to transfer the peak data info stored in every second.
//! @param endPos -> Number of peak data to be transfered .
//! @retval None.

void eventPeaksDetection::TransferPeakForPcDetection(const uint16_t endPos)
{
	for (uint16_t sendIndx = 0; sendIndx < endPos; sendIndx++)
	{
		apneaDetectionObj.FindOutAH(peakDataInSec.at(sendIndx), getRecWave);
	}
}

//! @brief The function shall reset the peak info every sec window.
//! @param None.
//! @retval None.

void eventPeaksDetection::ResetDataFrame(void)
{
	const uint16_t totalPeakIndex{static_cast<uint16_t>(peakDataInSec.size())};
	for (uint16_t sendIndx = 0; sendIndx < totalPeakIndex; sendIndx++)
	{
		peakDataInSec.at(sendIndx) = std::make_pair(0, 0.0);
	}
}
