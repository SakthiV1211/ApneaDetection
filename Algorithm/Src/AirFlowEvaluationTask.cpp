#include "AirFlowEvaluationTask.hpp"

//! @brief The function shall convert the current Date/Time to epoch time
//! format.
//! @param None.
//! @return gmt epotch time

uint32_t airFlowEvaluationTask::GetEpochTimeGMT(void)
{
  uint32_t currentEpochTime{static_cast<uint32_t>(std::mktime(&currentTime))};
  currentEpochTime =
      static_cast<uint32_t>(static_cast<int32_t>(currentEpochTime));
  return currentEpochTime;
}

void airFlowEvaluationTask::InitializeDateTime(std::string argStoredDate)
{
  // Find the position of the underscore
  size_t pos = argStoredDate.find('_');

  // Extract the date part (before the underscore)
  std::string dateStr = argStoredDate.substr(0, pos);

  // Extract the time part (after the underscore)
  std::string timeStr = argStoredDate.substr(pos + 1);

  // Extract date components: YYYY MM DD
  int16_t year = std::stof(dateStr.substr(2, 2));
  int16_t month = std::stof(dateStr.substr(4, 2));
  int16_t day = std::stof(dateStr.substr(6, 2));

  // Extract time components: HH MM SS
  int16_t hour = std::stof(timeStr.substr(0, 2));
  int16_t minute = std::stof(timeStr.substr(2, 2));
  int16_t second = std::stof(timeStr.substr(4, 2));

  // Output the results
  std::cout << "Date: " << dateStr << std::endl;
  std::cout << "Time: " << timeStr << std::endl;

  currentTime.tm_year = static_cast<int16_t>(2000U + year - 1900);
  currentTime.tm_mon = static_cast<int16_t>(month - 1U);
  currentTime.tm_mday = static_cast<int16_t>(day);
  currentTime.tm_hour = static_cast<int16_t>(hour);
  currentTime.tm_min = static_cast<int16_t>(minute);
  currentTime.tm_sec = static_cast<int16_t>(second);

  epotchTime = GetEpochTimeGMT();
}

//! @brief The function shall be used as an task handle for
//! airFlowEvaluationTask. The task handle shall wait untill the GUI Main task
//! is completed by sending the event bits from the respective task. Once the
//! event is synched the handle shall initialize the welch transform and then
//! the task handle shall acknowledge by sending the event bit. Afterwards the
//! task handle shall check for any queue triggered from examine task for every
//! 25 ms interval. The function shall process the state based on the message
//! received via queue.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::InitializeVar(void)
{
  welchObj.WelchTransformInit();
  ResetAFETaskVariable();
}

void airFlowEvaluationTask::EliminateOutliers(void)
{
  std::copy(recFlowPacket.begin(), recFlowPacket.end(), welchFlowInput.begin());
  HampelFilter<WELCH_FRAME_LENGTH, 5U>(welchFlowInput, recFlowPacket, 3);
}

//! @brief The function shall eliminate the DC voltage of the signal by
//! calculating the mean and subtracting the mean with the input data set. The
//! function shall compute the mean difference between the current and previous
//! input sets. If the absolute value of the mean difference exceeds 5, the
//! function shall signal a leakage detection to the event detection process.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::EliminateDcOffset(void)
{
  double previousMeanFlowWindow{meanFlowWindow};
  std::array<double, WELCH_FRAME_LENGTH> sortedData{welchFlowInput};
  std::sort(sortedData.begin(), sortedData.end());
  meanFlowWindow = {
      peaksForApneaDetection.apneaDetectionObj.mathAlgoObj.GetAverage(
          welchFlowInput.begin() + WELCH_FRAME_PERCENT,
          (WELCH_FRAME_LENGTH - (2 * WELCH_FRAME_PERCENT)))};
  const double standarDevOfSig{
      peaksForApneaDetection.apneaDetectionObj.mathAlgoObj.GetStandardDeviation(
          welchFlowInput.begin(), WELCH_FRAME_LENGTH)};
  debugFile << epochToHumanReadable(
                   epotchTime +
                   static_cast<uint16_t>(static_cast<float>(peakIndexTest) /
                                         SAMPLING_RATE))
            << " => Mean : " << meanFlowWindow << " , SD : " << standarDevOfSig
            << std::endl;
  const double meanDifference{meanFlowWindow - previousMeanFlowWindow};
  double dx{0.0};
  if (standarDevOfSig > 2.5)
  {
    dx = static_cast<double>(meanDifference / (WELCH_FRAME_LENGTH / 2.0));
  }
  if (std::abs(meanDifference) > 5.0)
  {
    peaksForApneaDetection.apneaDetectionObj.InitiatePcLeakageTune(
        meanDifference);
  }
  for (uint16_t index = static_cast<uint16_t>(SAMPLING_RATE * 5U);
       index < static_cast<uint16_t>(SAMPLING_RATE * 15U); index++)
  {
    previousMeanFlowWindow += dx;
    welchFlowInput.at(index) =
        ((welchFlowInput.at(index) - previousMeanFlowWindow));
  }
}

//! @brief The function shall check whether the mean data exceed 50 for two
//! consecutive instances. If it exceeds twice, the function shall send message
//! to GUI task to stop the therapy. The function shall stop validating the flow
//! rate if the validateBit is set to true. The validateBit is set to true when
//! auto start is triggered and it shall switch back to false after one minute.
//! @param argMeanData -> Mean data of a sample flow set.
//! @retval None.

void airFlowEvaluationTask::ValidateOpenAirflow(const double argMeanData)
{
  if ((argMeanData >= static_cast<double>(50U)) && (validateBit == false))
  {
    leakageRepeativeCount++;
    if (leakageRepeativeCount >= 2U)
    {
      // printf("LeakageDetected %.2f\n", argMeanData);
      leakageRepeativeCount = 0U;
    }
  }
  else
  {
    leakageRepeativeCount = 0U;
  }
}

//! @brief The function shall examine the airflow data based on the therapy
//! handler when the occupied packet is full. Once examine the flow data, the
//! function shall shift the flow window for 10 secs.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::ExamineAirflowData(void)
{
  if (occupiedPackets >= 4U)
  {
    EliminateOutliers();
    EliminateDcOffset();
    TherapyFlowProcess();
    (void)std::copy(recFlowPacket.begin() + (SAMPLE_FRAME_LENGTH * 2U),
                    recFlowPacket.end(), recFlowPacket.begin());
    occupiedPackets = 2U;
  }
}

//! @brief The function shall identify the peak data for every second. If no
//! peak is detected the function shall send the info to AHI detection. The
//! function shall calculate the leakage rate and negative peak average.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::DetectPeaksInfo(void)
{
  for (uint16_t selectRangeIndex = static_cast<uint16_t>(SAMPLING_RATE * 5U);
       selectRangeIndex < static_cast<uint16_t>(SAMPLING_RATE * 15U);
       selectRangeIndex++)
  {
    // const double filteredFlowRateForEventDet{
    //     filterDesignForAHI.FilterEquation(welchFlowInput.at(selectRangeIndex))};
    sgInputData.CircularDataAppend(welchFlowInput.at(selectRangeIndex));
    const double sgFilterData{SavitzkyGolayFilter(sgInputData.dataFrame)};

    peaksForApneaDetection.FlowRatePeakDetectionMethod(sgFilterData);
    printFile << peaksForApneaDetection.apneaDetectionObj.mathAlgoObj
                     .RoundUpMethod(sgFilterData, 3U)
              << std::endl;
  }
  peaksForApneaDetection.ProcessPeakData(deriveSignalWave);
}

//! @brief The function shall receive the flow data from the queue and store the
//! data in the local array.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::ProcessFlowData(const float *flowWindow)
{
  (void)std::copy_n(
      flowWindow, SAMPLE_FRAME_LENGTH,
      recFlowPacket.begin() +
          (occupiedPackets * static_cast<uint32_t>(SAMPLE_FRAME_LENGTH)));
  ++occupiedPackets;
  ExamineAirflowData();
}

//! @brief The function state handler shall be invoked when the therapy is on.
//! The function shall check for any leakage/blockage test during therapy run.
//! The function shall identify the waveform and find peaks for apnea event
//! detection.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::TherapyFlowProcess(void)
{
  ValidateOpenAirflow(meanFlowWindow);
  deriveSignalWave = PowerSpectra();
  deriveSignalWave = waveform::sine_wave;
  // peaksForApneaDetection.apneaDetectionObj.PrintWaveform(deriveSignalWave);
  DetectPeaksInfo();
}

//! @brief The function shall calculate the power spectrum of the flow data
//! using welch transform. After applying power spectrum the function shall
//! identify the waveform using relative power formula.
//! @param argCompValue -> Cutoff value for detecting the waveform.
//! @retval Breathe waveform.

waveform airFlowEvaluationTask::PowerSpectra(void)
{
  std::array<double, WELCH_FRAME_LENGTH> freqAnalyzisSignal{welchFlowInput};
  windowFunction hanWinObj;
  hanWinObj.HanningWindow(freqAnalyzisSignal, WELCH_FRAME_LENGTH);
  welchObj.ComputeWelchTransform(
      freqAnalyzisSignal, static_cast<uint16_t>(HANNING_WINDOW_LENGTH),
      static_cast<uint16_t>(OVERLAP_WELCH_LENGTH), true);
  return welchObj.DetectFlowStatus();
}

//! @brief The function shall reset the data members of the class.
//! @param None.
//! @retval None.

void airFlowEvaluationTask::ResetAFETaskVariable(void)
{
  cmfLeakRate = 0.0;
  occupiedPackets = 0U;
  oneMinCounter = 0U;
  incrementCounter = 0U;
  leakageRepeativeCount = 0U;
  recFlowPacket.fill(0U);
  welchFlowInput.fill(0U);

  peaksForApneaDetection.ResetEventPeakInfoData();
  meanFlowWindow = 0.0;
  meanWindowIndex = 0U;
  sgInputData.ResetFrameSeg();
  sgInputData.frameIndex = SG_NUMBER;
}
