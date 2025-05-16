/*
 *
 *  Created on: 1-Aug-2022
 *      Author: c11002
 */

#ifndef INC_APNEADETECTION_HPP_
#define INC_APNEADETECTION_HPP_

#include "AhiDefine.hpp"
#include "DataSampleInc.hpp"
#include "MathAlgo.hpp"
#include "algorithm"
#include "cstring"

using namespace ahiDefineBound;
using namespace dataSampleBound;

class apneaDetection {
private:
  bool centralApneaDetected;
  bool isReferenceComplete;
  uint16_t addOnCutOffCount;
  uint16_t autoTunePcCount;
  uint16_t packetTimeSize;
  uint16_t differenceCount;
  double bwp;
  pcInfoContainer pcInfoObj;
  thersholdContainer thresholdLevel;
  window frame;
  countInfo peaksCountInfo;
  pcTest pcTestInfo;
  windowPeaks peaksCountNum;
  std::array<double, 50U> pcBufferArray;

  void (apneaDetection::*ShiftWinHandler)(void);
  std::array<void (apneaDetection::*)(double), 4U> pcTestHandler;

  uint16_t CutOffNumInRange(const double, const double);
  double FindMax(void);
  void DetectAlgo(const uint16_t, const double);
  void ValidateApneaHypopnea(void);

  void AppendPcBuffer(const double);
  bool InitialPcDifference(const double);
  bool PcDataComparision(const double, const double, const uint16_t);
  void PcBufferSegmentCheck(void);

  void SortReferenceWindowInDescendingOrder(void);
  void AlignWindowSegment();
  void CountsForWindowAlign();

  void ShiftPositivePeakWindow(void);
  void ShiftOnlyMetricWindow(void);
  void ResetWindowFrame(double *, const uint16_t);

  void CountValueAfterShift(const uint16_t);
  void ShiftArrayCount(const uint16_t);
  void SetZeroToFrame(void);

  void CheckPcExceedsCutOff(const double);
  void AppendFivePCValues(const double);
  void ExaminePcData(const double);
  void WaitUntillPcSettle(const double);

  void DiagnosisPcValues(const double);
  double CalculatePcValue(void);

  void AllowDataToBaseWindow(void);
  void AddPeakDataCheck(const double, const waveform);
  double GetPeakShiftData(const double, const waveform);
  void RearrangePcArray(const uint16_t);

public:
  mathAlgo mathAlgoObj{};
  apneaDetection();

  void FindOutAH(const std::pair<uint16_t, double>, const waveform);
  void ResetAhiValues();
  void CentralApneaTrigMsg(void);
  void EventTrigMsg(void);
  void InitiatePcLeakageTune(const double);
  void PrintWaveform(const waveform printWave);
};

#endif /* INC_APNEADETECTION_HPP_ */
