/*
 * FindingPeaks.hpp
 *
 *  Created on: 26-Jul-2022
 *      Author: c11002
 */

#ifndef INC_EVENTPEAKSDETECTION_HPP_
#define INC_EVENTPEAKSDETECTION_HPP_

#include <array>
#include "FindingPeaksInclude.hpp"
#include "MathAlgo.hpp"
#include "DataSampleInc.hpp"
#include "FramePacket.hpp"
#include "ApneaDetection.hpp"

using namespace peakDefine;
/**
 * @class eventPeaksDetection
 * @brief The class used for event peak deduction.
 */
class eventPeaksDetection
{
  private:
	extendedSecsState extendSecs;
	waveform getRecWave;
	uint8_t negPeakCount;
	uint16_t positionIndex;
	uint16_t peakIndexPosition;
	uint16_t intervalGapAdjCounter;
	uint16_t maxSlopeCounter;
	const double slopeFactorConst;
	double peakBuffer;
	double peakSlopeDiff;
	peakDetectContainers eventPeakDetectVar;

	std::array<bool (eventPeaksDetection::*)(void), 2U> basicDetectionHandler;
	std::array<void (eventPeaksDetection::*)(void), 2U> reconfirmDetectionHandler;

	framePacket<double, 3U> flowRateFrame;
	framePacket<double, 10U> peakFrame;
	framePacket<double, 10U> valleyFrame;
	framePacket<uint16_t, 10U> peakIndexFrame;
	std::array<std::pair<uint16_t, double>, 10> peakDataInSec;

	void EventPeaksIntrevalGap(void);

	bool PositivePeakBasicCheck(void);
	bool NegativePeakBasicCheck(void);

	void PositivePeakReconfirmCheck(void);
	void NegativePeakReconfirmCheck(void);

	bool ConfirmationCheckTime(void);
	void ReConfirmPeak(void);

	void SendNegativePeaksForPC(void);
	void GetPeaksInfoForEverySec(void);
	void TransferPeakValidation(void);
	void TransferPeakForPcDetection(const uint16_t);
	void ResetDataFrame(void);
	void CheckForPrevSecs(void);

  public:
	eventPeaksDetection();
	apneaDetection apneaDetectionObj;

	void ResetEventPeakInfoData(void);
	void FlowRatePeakDetectionMethod(const double);
	void ProcessPeakData(const waveform, const bool);
};

#endif /* INC_EVENTPEAKSDETECTION_HPP_ */
