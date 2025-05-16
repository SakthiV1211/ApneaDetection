/*
 * AhiIndex.cpp
 *
 *  Created on: 1-Aug-2022
 *      Author: c11002
 */

#include "ApneaDetection.hpp"
#include "stdio.h"
#include <iomanip>

void apneaDetection::PrintWaveform(const waveform printWave)
{
  // debugFile << epochToHumanReadable(epotchTime +
  // static_cast<uint16_t>(static_cast<float>(peakIndexTest)/SAMPLING_RATE))
  // << " => Waveform : " << static_cast<uint16_t>(printWave)
  // << std::endl;
}

//! @brief Default constructor. The function shall initialize the data members
//! of the class.

apneaDetection::apneaDetection()
    : centralApneaDetected{false}, isReferenceComplete{false},
      addOnCutOffCount{0u}, autoTunePcCount{0U}, packetTimeSize{0U},
      differenceCount{0U}, bwp{0.0},
      pcInfoObj{0.0, 0.0}, thresholdLevel{0.0}, frame{{0U}, {0U}, {0U}},
      peaksCountInfo{0U, 0U, 0U, {0U}},
      pcTestInfo{pcState::check_pc_surpassed_cut_off, 0U, {0.9, 0.7}},
      peaksCountNum{}, pcBufferArray{0U},
      ShiftWinHandler{&apneaDetection::ShiftPositivePeakWindow}, pcTestHandler{
                                                                     nullptr}
{
  pcTestHandler.at(static_cast<int8_t>(pcState::check_pc_surpassed_cut_off)) =
      &apneaDetection::CheckPcExceedsCutOff;
  pcTestHandler.at(static_cast<int8_t>(pcState::append_five_pc_data)) =
      &apneaDetection::AppendFivePCValues;
  pcTestHandler.at(static_cast<int8_t>(pcState::append_and_examine_pc_data)) =
      &apneaDetection::ExaminePcData;
  pcTestHandler.at(static_cast<int8_t>(pcState::wait_pc_to_settle)) =
      &apneaDetection::WaitUntillPcSettle;
}

//! @brief The function shall be used to initialize the leakage factor for event
//! detection when leakage is detected during the sampling process.
//! @param argMeanDiff -> Mean difference between current and previous sample
//! window.
//! @retval None.

void apneaDetection::InitiatePcLeakageTune(const double argMeanDiff)
{
  if (isReferenceComplete == true)
  {
    // debugFile << epochToHumanReadable(epotchTime +
    // static_cast<uint16_t>(static_cast<float>(peakIndexTest)/SAMPLING_RATE))
    //           << " => *** InitiatePcLeakageTune "
    //           << mathAlgoObj.RoundUpMethod(argMeanDiff, 3) << std::endl;
  }
}

//! @brief The function shall calculate the total number of pc counts range
//! between lower limit and upper limit.
//! @param lowerLimit -> Lower limit range.
//! @param upperLimit -> Maximum limit range.
//! @retval number of counts

uint16_t apneaDetection::CutOffNumInRange(const double lowerLimit,
                                          const double upperLimit)
{
  uint16_t counter{0U};
  const uint16_t totalPcCount{static_cast<uint16_t>(pcTestInfo.pcCount)};
  for (uint16_t index = 0U; index < totalPcCount; index++)
  {
    if ((pcBufferArray.at(index) >= lowerLimit) &&
        (pcBufferArray.at(index) < upperLimit))
    {
      counter++;
    }
  }
  return counter;
}

//! @brief The function shall find the maximum value of the pc data from the
//! buffer array.
//! @param None.
//! @retval Maximum value in double.

double apneaDetection::FindMax(void)
{
  double max{0.0};
  const uint16_t totalPcCount{static_cast<uint16_t>(pcTestInfo.pcCount)};
  for (uint16_t index = 0U; index < totalPcCount; index++)
  {
    if (pcBufferArray.at(index) > max)
    {
      max = pcBufferArray.at(index);
    }
  }
  return max;
}

//! @brief The function shall indicate when central apnea is identified from the
//! flow data.
//! @param None.
//! @retval None

void apneaDetection::CentralApneaTrigMsg(void) { centralApneaDetected = true; }

//! @brief The function shall update the event comparision value if the apnea
//! start event is triggered from examine flow task.
//! @param None.
//! @retval None.

void apneaDetection::EventTrigMsg(void)
{
  addOnCutOffCount = 2U;
}

//! @brief The function shall identify the apnea/hypopnea events based on the
//! argument input.The function shall process two stages of test for
//! apnea/hypopnea detection. In stage 1 the function shall able to distinguish
//! apnea or no apnea by comparing the total number of pc count and pc range
//! count. If the counts exceed the respective cut off count the function shall
//! proceed to stage 2 for apnea/ hypopnea detection otherwise it shall declare
//! it as no apnea. In stage 2 the apnea or hypopnea identifcation shall be
//! based on the maximum pc data in the buffer array. If any event detected the
//! function shall send the message to GUI task.
//! @param argPcRangeCount -> Pc buffer range count.
//! @param maxPcVal -> Maximum pc value in the buffer array.
//! @retval None.

void apneaDetection::DetectAlgo(const uint16_t argPcRangeCount,
                                const double maxPcVal)
{
  if ((pcTestInfo.pcCount >= (17U - addOnCutOffCount)) &&
      (argPcRangeCount >= (14U - addOnCutOffCount)))
  {
    if (mathAlgoObj.EpsilonCheck(maxPcVal, pcTestInfo.pcLimRange.at(0U)) ==
        true)
    {
      differenceCount = 0U;
      pcTestInfo.pcCurrentState = pcState::wait_pc_to_settle;
      if (centralApneaDetected == true)
      {
        // debugFile << " => --Central Apnea -- ";
      }
      else
      {
        debugFile << " -- Apnea -- ";
        outFile << peakIndexTest << ",-Apnea-,"
                << epochToHumanReadable(
                       epotchTime +
                       static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                             SAMPLING_RATE))
                << "\n";
        apneaCount++;
      }
    }
    else if (mathAlgoObj.EpsilonCheck(maxPcVal,
                                      pcTestInfo.pcLimRange.at(1U)) == true)
    {
      differenceCount = 0U;
      pcTestInfo.pcCurrentState = pcState::wait_pc_to_settle;
      if (centralApneaDetected == true)
      {
      }
      else
      {
        debugFile << " => -- Hypopnea -- ";
        outFile << peakIndexTest << ",-Hypopnea-,"
                << epochToHumanReadable(
                       epotchTime +
                       static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                             SAMPLING_RATE))
                << "\n";
        hypoApneaCount++;
      }
    }
    else
    {
    }
  }
  addOnCutOffCount = 0U;
  centralApneaDetected = false;
}

//! @brief The function shall validate apnea/hypopnea event by calculating the
//! maximum pc value and range count in the buffer array. The function shall
//! send the info for validation.
//! @param None.
//! @retval None.

void apneaDetection::ValidateApneaHypopnea(void)
{
  double maxPcValInSeg{FindMax()};
  pcInfoObj.pcCompareValue = maxPcValInSeg;

  uint16_t pcRangeCount{
      CutOffNumInRange((maxPcValInSeg * 0.35), maxPcValInSeg + 0.1)};
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => ValidateApneaHypopnea Tot :"
            << static_cast<uint16_t>(pcTestInfo.pcCount)
            << " Mid : " << pcRangeCount;
  debugFile << ", " << mathAlgoObj.RoundUpMethod(maxPcValInSeg, 3);
  DetectAlgo(pcRangeCount, maxPcValInSeg);
  debugFile << std::endl;
}

//! @brief The function shall determine the count value where the adjacent PC
//! data difference is less than 0.3 and shift the buffer window up to that
//! count value.
//! @param pcLen -> Count value to shift the data.
//! @retval None.

void apneaDetection::RearrangePcArray(const uint16_t pcLen)
{
  pcTestInfo.pcCount =
      static_cast<uint8_t>(static_cast<uint32_t>(pcBufferArray.size()) -
                           static_cast<uint32_t>(pcLen));
  (void)std::copy(pcBufferArray.begin() + pcLen, pcBufferArray.end(),
                  pcBufferArray.begin());
  const uint16_t pcBuffMaxSize{static_cast<uint16_t>(pcBufferArray.size())};
  for (uint16_t startPos = pcTestInfo.pcCount; startPos < pcBuffMaxSize;
       startPos++)
  {
    pcBufferArray.at(startPos) = 0.0;
  }
}
//! @brief The function shall be invoked when the PC frame packet becomes full
//! during the examine PC detection state. It shall restart the PC detection
//! state if the maximum PC value is less than 0.2 and the adjacent PC data
//! difference is less than 0.3 for 18 consecutive occurrences. Otherwise, the
//! function shall rearrange the PC buffer array.
//! @param None.
//! @retval None.

void apneaDetection::PcBufferSegmentCheck(void)
{
  uint16_t pcDiffCount{0U};
  const uint16_t totalPcCount{static_cast<uint16_t>(pcTestInfo.pcCount)};
  for (uint16_t pcPosition = 1u; pcPosition < totalPcCount; pcPosition++)
  {
    const double pcTestDiff{std::abs(pcBufferArray.at(pcPosition) -
                                     pcBufferArray.at(pcPosition - 1u))};
    if (pcTestDiff < 0.06)
    {
      ++pcDiffCount;
    }
    else
    {
      pcDiffCount = 0U;
    }
  }
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => PcDiffCount " << pcDiffCount << std::endl;
  if ((pcDiffCount > 18U) && (FindMax() < 0.4))
  {
    AllowDataToBaseWindow();
  }
  else
  {
    RearrangePcArray(pcDiffCount);
  }
}

//! @brief The function shall compare the current pc data and reference data. If
//! the current data is less than refernce data for five consecutive 5 times
//! then the function shall return false, otherwise it returns true.
//! @param argPcData -> Current pc data.
//! @param comparisionData -> Reference data.
//! @retval true/false

bool apneaDetection::PcDataComparision(const double argPcData,
                                       const double comparisionData,
                                       const uint16_t compCount)
{
  if (mathAlgoObj.EpsilonCheck(comparisionData, argPcData) == true)
  {
    differenceCount++;
    if (differenceCount >= compCount)
    {
      differenceCount = 0U;
      return true;
    }
  }
  else
  {
    differenceCount = 0U;
  }
  return false;
}

//! @brief The function shall return true if the difference between the current
//! and previous PC data is less than 0.05, otherwise it shall return false.
//! @param pcAppend -> The current pc data.
//! @retval true/false

bool apneaDetection::InitialPcDifference(const double argPcData)
{
  const double differenceInPc{pcInfoObj.previousPcData - argPcData};
  if (mathAlgoObj.EpsilonCheck(differenceInPc, 0.1) == true)
  {
    differenceCount = 0U;
    return true;
  }
  return false;
}

//! @brief The function shall append the normalized pc data in the buffer array.
//! The function shall perform the circular buffer to prevent overflow.
//! @param pcAppend -> The current pc data.
//! @retval None.

void apneaDetection::AppendPcBuffer(const double pcAppend)
{
  if (pcTestInfo.pcCount == pcBufferArray.size())
  {
    (void)std::copy(pcBufferArray.begin() + 1U, pcBufferArray.end(),
                    pcBufferArray.begin());
    --pcTestInfo.pcCount;
  }
  pcBufferArray.at(pcTestInfo.pcCount) = pcAppend;
  pcTestInfo.pcCount++;
}

//! @brief The function shall start allowing the peak data from metric window to
//! base line window. The function shall restart appending the negtaive peak
//! data by setting the negCheck to true.
//! @param None.
//! @retval None.

void apneaDetection::AllowDataToBaseWindow(void)
{
  ShiftWinHandler = &apneaDetection::ShiftPositivePeakWindow;
  pcTestInfo.pcCurrentState = pcState::check_pc_surpassed_cut_off;
}

//! @brief The function shall wait until the PC value reaches approximately the
//! baseline cutoff value or exceeds the mid PC data for five consecutive
//! occurrences. Once this condition is met, the function shall reset the PC
//! detection variables.
//! @param argPcData -> Current Pc Data
//! @retval None.

void apneaDetection::WaitUntillPcSettle(const double argPcData)
{
  if ((argPcData <= thresholdLevel.baseCutOff) ||
      (PcDataComparision((argPcData), (pcInfoObj.pcCompareValue), 40U) ==
       true))
  {
    AllowDataToBaseWindow();
  }
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => WaitUntillPcSettle : "
            << mathAlgoObj.RoundUpMethod(argPcData, 3) << " , MidPcCutOff : "
            << mathAlgoObj.RoundUpMethod(pcInfoObj.pcCompareValue, 3)
            << std::endl;
  pcInfoObj.previousPcData = argPcData;
}

//! @brief The function shall analyze PC data only after the reference window is
//! complete. It shall monitor the difference between the current and previous
//! PC data. If the difference exceeds 0.03, the function shall initiate
//! apnea/hypopnea detection by starting to append PC data to the buffer.
//! Additionally, it shall stop appending negative peaks by setting negCheck to
//! stop_append and halt the transmission of data from the metric to the
//! baseline window. The function shall also calculate the baseline cutoff PC
//! data and examine PC data based on leakage information for further
//! validation.
//! @param argPcData -> Current Pc Data
//! @retval None.

void apneaDetection::CheckPcExceedsCutOff(const double argPcData)
{
  if (isReferenceComplete == true)
  {
    const double differenceInPc{argPcData - pcInfoObj.previousPcData};
    if (mathAlgoObj.EpsilonCheck(differenceInPc, 0.06) == true)
    {
      pcBufferArray.fill(0U);
      pcTestInfo.pcCount = 0U;
      differenceCount = 0U;
      thresholdLevel.baseCutOff = 0.25;
      AppendPcBuffer(argPcData);
      pcTestInfo.pcCurrentState = pcState::append_five_pc_data;
      ShiftWinHandler = &apneaDetection::ShiftOnlyMetricWindow;
    }
  }
  pcInfoObj.previousPcData = argPcData;
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => CheckPcExceedsCutOff : "
            << mathAlgoObj.RoundUpMethod(argPcData, 3) << std::endl;
}

//! @brief The function shall append the next five PC data points to the PC
//! buffer array. Additionally, it shall stop PC detection and start fresh if
//! the difference between the current and previous PC data is less than 0.05.
//! @param argPcData -> Current Pc Data
//! @retval None.

void apneaDetection::AppendFivePCValues(const double argPcData)
{
  if (InitialPcDifference(argPcData) == true)
  {
    pcInfoObj.previousPcData = argPcData;
    pcTestInfo.pcCurrentState = pcState::check_pc_surpassed_cut_off;
    ShiftWinHandler = &apneaDetection::ShiftPositivePeakWindow;
    return;
  }
  AppendPcBuffer(argPcData);
  pcInfoObj.previousPcData = argPcData;
  if (pcTestInfo.pcCount >= 6U)
  {
    differenceCount = 0;
    pcTestInfo.pcCurrentState = pcState::append_and_examine_pc_data;
    std::array<double, 6> pcCopy;
    std::copy_n(pcBufferArray.begin(), 6U, pcCopy.begin());
    std::sort(pcCopy.begin(), pcCopy.end());
    thresholdLevel.examineCutOff =
        ((pcCopy.at(0U) + pcCopy.at(1U)) / 2.0) + 0.1;
  }
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => AppendFivePCValues : "
            << mathAlgoObj.RoundUpMethod(argPcData, 3) << std::endl;
}

//! @brief The function shall append PC data points to the buffer array. If the
//! data points reach the maximum limit, the PC data falls below the baseline
//! cutoff, or the PC data remains below the examine cutoff for five consecutive
//! occurrences, the function shall validate the buffer array for event
//! detection. Additionally, it shall reconfigure data appending when the PC
//! data drops below the baseline cutoff or stays below the examine cutoff for
//! five consecutive occurrences.
//! @param argPcData -> Current Pc Data
//! @retval None.

void apneaDetection::ExaminePcData(const double argPcData)
{
  AppendPcBuffer(argPcData);
  if (argPcData <= thresholdLevel.baseCutOff)
  {
    ValidateApneaHypopnea();
    AllowDataToBaseWindow();
  }
  else if (pcTestInfo.pcCount >= pcBufferArray.size())
  {
    ValidateApneaHypopnea();
    PcBufferSegmentCheck();
  }
  else
  {
    if (PcDataComparision(argPcData, thresholdLevel.examineCutOff, 4U) ==
        true)
    {
      ValidateApneaHypopnea();
      AllowDataToBaseWindow();
    }
  }
  pcInfoObj.previousPcData = argPcData;
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => ExaminePcData : " << mathAlgoObj.RoundUpMethod(argPcData, 3)
            << " ,ExamineCutOff : "
            << mathAlgoObj.RoundUpMethod(thresholdLevel.examineCutOff, 3)
            << std::endl;
}

//! @brief The state handler for processing PC data. The function shall extend
//! the holding period for appending negative data and update the new negative
//! average when negCheckState is set to the wait_append state. Additionally,
//! it shall decrement the leakageStructObj.leakageCorrectionCount until it
//! reaches zero if leakage is detected.
//! @param argPcData -> Current Pc Data
//! @retval None.

void apneaDetection::DiagnosisPcValues(const double pcData)
{
  (this->*pcTestHandler.at(
              static_cast<std::size_t>(pcTestInfo.pcCurrentState)))(pcData);
}

//! @brief The function shall sort the baseline reference window in descending
//! order.
//! @param None.
//! @retval None.

void apneaDetection::SortReferenceWindowInDescendingOrder(void)
{
  std::sort(frame.baseLineWindow.begin(),
            frame.baseLineWindow.begin() + peaksCountNum.baseLineWindow,
            std::greater<double>());
}

//! @brief The functin shall separate the 130 secs peak data in to two
//! segments. The first segment consists of 120 secs peak data, will be named
//! as basesine reference window. The second segment has the latest 10 secs
//! peak data, will be named as metric window.
//! @param None.
//! @retval None.

void apneaDetection::AlignWindowSegment(void)
{
  (void)std::copy_n(frame.positivePeakWindow.begin(),
                    peaksCountNum.baseLineWindow, frame.baseLineWindow.begin());
  (void)std::copy_n(frame.positivePeakWindow.begin() +
                        peaksCountNum.baseLineWindow,
                    peaksCountNum.metricWindow, frame.metricWindow.begin());

  SortReferenceWindowInDescendingOrder();
}

//! @brief The function shall calculate the total number of peaks in the
//! baseline window and metric window.
//! @param None.
//! @retval None.

void apneaDetection::CountsForWindowAlign(void)
{
  (void)memset(&peaksCountNum, 0U, sizeof(peaksCountNum));

  uint16_t size = static_cast<uint16_t>(peaksCountInfo.peaksArray.size());
  int16_t metricLen{10};
  while (metricLen > 0)
  {
    peaksCountNum.metricWindow += static_cast<uint32_t>(
        peaksCountInfo.peaksArray.at(static_cast<std::size_t>(--size)));
    metricLen = static_cast<int16_t>(
        metricLen - static_cast<int32_t>(WINDOW_SHIFT_IN_SECS));
  }
  peaksCountNum.baseLineWindow = static_cast<uint16_t>(
      static_cast<uint32_t>(peaksCountInfo.countValueIndex) -
      static_cast<uint32_t>(peaksCountNum.metricWindow));
}

//! @brief The function shall calculate the Pc data. The function shall
//! seperate the whole window in to metric and reference window. The bwp shall
//! be calculated by averaging the 45% data of reference window sorted in
//! descending order. The bwm shall be calculated by averaging the metric
//! window. The pc data shall be calculate from the bwp and bwm data.
//! @param None.
//! @retval cuurent pc data.

double apneaDetection::CalculatePcValue(void)
{
  CountsForWindowAlign();
  AlignWindowSegment();
  const uint16_t lf{static_cast<uint16_t>(
      0.45 * static_cast<double>(peaksCountNum.baseLineWindow))};

  bwp = mathAlgoObj.GetAverage(frame.baseLineWindow.begin() + 3U, lf);
  double bwm{mathAlgoObj.GetAverage(frame.metricWindow.begin(),
                                    peaksCountNum.metricWindow)};

  double pc{0.0};
  if (std::abs(bwp) >= 1e-6)
  {
    pc = ((bwp - bwm) / bwp);
  }
  return pc;
}

//! @brief The function shall perform the circular buffer by removing the peak
//! in the selected peak element address and shifting the whole window back
//! for one secs. The function shall decrement the counter value by one.
//! @param
//! @retval None.

void apneaDetection::ResetWindowFrame(double *windowPtr,
                                      const uint16_t oneSecChopCount)
{
  peaksCountInfo.countValueIndex -= static_cast<uint32_t>(oneSecChopCount);

  (void)std::copy_n(windowPtr + oneSecChopCount, peaksCountInfo.countValueIndex,
                    windowPtr);
  for (uint32_t resetIndex = peaksCountInfo.countValueIndex; resetIndex < 130U;
       resetIndex++)
  {
    frame.positivePeakWindow.at(resetIndex) = 0U;
  }
}

//! @brief The function shall shift the window for one secs based on the start
//! index input
//! @param startIndex -> 0 Shift whole window, 120 shift only metric window.
//! @retval None.

void apneaDetection::ShiftArrayCount(const uint16_t startIndex)
{
  (void)std::copy(peaksCountInfo.peaksArray.begin() + startIndex + 1,
                  peaksCountInfo.peaksArray.end(),
                  peaksCountInfo.peaksArray.begin() + startIndex);
}

//! @brief The function shall reset the reference and metric window to zero.
//! @param None.
//! @retval None.

void apneaDetection::SetZeroToFrame(void)
{
  frame.metricWindow.fill(0U);
  frame.baseLineWindow.fill(0U);
}

//! @brief The function shall shift the whole window while the pc data
//! algorithm is waiting to start detection.
//! @param None.
//! @retval None.

void apneaDetection::ShiftPositivePeakWindow(void)
{
  if (peaksCountInfo.peaksArray.at(0U) != 0U)
  {
    ResetWindowFrame(frame.positivePeakWindow.begin(),
                     peaksCountInfo.peaksArray.at(0U));
  }
  ShiftArrayCount(0);
  --packetTimeSize;
  SetZeroToFrame();
}

//! @brief The function shall shift the metric window when the pc data
//! algorithm is in the processing state.
//! @param None.
//! @retval None.

void apneaDetection::ShiftOnlyMetricWindow(void)
{
  if (peaksCountInfo.peaksArray.at(120U) != 0U)
  {
    ResetWindowFrame(frame.positivePeakWindow.begin() +
                         peaksCountNum.baseLineWindow,
                     peaksCountInfo.peaksArray.at(120U));
  }
  ShiftArrayCount(120);
  --packetTimeSize;
  SetZeroToFrame();
}

//! @brief The function shall retrieve peak data information for a second,
//! indicating whether a peak is present and its value. It shall append the
//! peak presence information for each second to the sample window. If the
//! sample window becomes full, the function shall diagnose PC data.
//! @param positivePeakInfo -> Peak data info for a second.
//! std::get<0>(positivePeakInfo) = 1
//! ->Positive peak detected. std::get<0>(positivePeakInfo) = 0 ->No positive
//! peak found.
//! @param recordedWaveOp -> Breathe waveform of current sample window.
//! @retval None.

void apneaDetection::FindOutAH(
    const std::pair<uint16_t, double> positivePeakInfo,
    const waveform recordedWaveOp)
{
  if (packetTimeSize == SAMPLE_WINDOW_SECS)
  {
    DiagnosisPcValues(CalculatePcValue());
    (this->*ShiftWinHandler)();
    isReferenceComplete = true;
  }
  peaksCountInfo.peaksArray.at(packetTimeSize) = positivePeakInfo.first;
  if (positivePeakInfo.first != 0U)
  {
    AddPeakDataCheck(positivePeakInfo.second, recordedWaveOp);
  }
  packetTimeSize++;
}

//! @brief The function shall be used to append the peak data to the window
//! segment. It shall ignore the peak data if the reference window is not
//! complete or if the current breath waveform is noisy.
//! @param argActualPeak -> Positive peak data.
//! @param argRecWave -> Breathe waveform of current sample window.
//! @retval None.

void apneaDetection::AddPeakDataCheck(const double argActualPeak,
                                      const waveform argRecWave)
{
  if (isReferenceComplete == false)
  {
    frame.positivePeakWindow.at(peaksCountInfo.countValueIndex) = argActualPeak;
    peaksCountInfo.countValueIndex +=
        static_cast<uint32_t>(peaksCountInfo.peaksArray.at(packetTimeSize));
  }
  else
  {
    frame.positivePeakWindow.at(peaksCountInfo.countValueIndex) =
        GetPeakShiftData(argActualPeak, argRecWave);
    peaksCountInfo.countValueIndex +=
        static_cast<uint32_t>(peaksCountInfo.peaksArray.at(packetTimeSize));
  }
}

//! @brief The function shall be used for peak data normalization by
//! subtracting the current peak from the negative average. If the current
//! waveform is noisy, the normalized value shall be updated to BWP (average
//! reference window) to eliminate unwanted spikes. Additionally, the function
//! shall auto-adjust the normalized peak data if it exceeds BWP * 1.075 or
//! falls below BWP
//! * 0.485, ensuring it remains within these bounds for improved event
//! detection. The function shall also adjust the peak data when leakage is
//! detected.
//! @param actualPeak -> Current peak data.
//! @param argRecWave -> Breathe waveform of current sample window.
//! @retval normalized peak data.

double apneaDetection::GetPeakShiftData(const double actualPeak,
                                        const waveform argRecWave)
{
  double peakShiftData{actualPeak};
  if (argRecWave == waveform::other_freq_wave)
  {
    peakShiftData = bwp;
  }
  else
  {
    if (peakShiftData < 0.0)
    {
      peakShiftData = 0.0;
    }
    else if (peakShiftData > (bwp * 1.2))
    {
      peakShiftData = bwp * 1.2;
    }
    else
    {
    }
  }
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => GetPeakShiftData : "
            << mathAlgoObj.RoundUpMethod(actualPeak, 3)
            << " , " << peakShiftData << std::endl;
  return peakShiftData;
}

//! @brief The function shall reset all data members of the apneaDetection
//! class.
//! @param None.
//! @retval None.

void apneaDetection::ResetAhiValues(void)
{
  addOnCutOffCount = 0u;
  bwp = 0.0;
  centralApneaDetected = false;
  packetTimeSize = 0U;
  isReferenceComplete = false;
  differenceCount = 0U;
  (void)memset(&peaksCountInfo, 0U, sizeof(peaksCountInfo));
  (void)memset(&peaksCountNum, 0U, sizeof(peaksCountNum));
  (void)memset(&frame, 0U, sizeof(frame));

  pcBufferArray.fill(0);
  pcTestInfo.pcCount = 0U;
  pcTestInfo.pcCurrentState = pcState::check_pc_surpassed_cut_off;

  ShiftWinHandler = &apneaDetection::ShiftPositivePeakWindow;

  if (!outFile.is_open())
  {
    printf("Error opening file \n");
  }

  if (!debugFile.is_open())
  {
    printf("Debug - Error opening file \n");
  }

  if (!printFile.is_open())
  {
    printf("Debug - Error opening file \n");
  }

  if (!peakPosPrint.is_open())
  {
    printf("Debug - Error opening file \n");
  }
}
