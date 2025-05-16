#ifndef INC_AIRFLOWEVALUATIONTASK_HPP
#define INC_AIRFLOWEVALUATIONTASK_HPP

#include "EventPeaksDetection.hpp"
#include "FIRFilter.hpp"
#include "FilterCoeffDefine.hpp"
#include "WelchTransform.hpp"
#include <array>
#include <cstring>
#include "HampelFilter.hpp"
#include "SavitzkyGolayFilter.hpp"

class airFlowEvaluationTask {
private:
  bool validateBit;
  bool blockNegativePeakAppend;
  waveform deriveSignalWave;
  uint8_t incrementCounter;
  uint8_t occupiedPackets;
  uint8_t oneMinCounter;
  uint8_t leakageRepeativeCount;
  uint8_t meanWindowIndex;
  double meanFlowWindow;
  double cmfLeakRate;
  std::array<double, WELCH_FRAME_LENGTH> recFlowPacket;
  std::array<double, WELCH_FRAME_LENGTH> welchFlowInput;
  framePacket<double, SG_NUMBER> sgInputData;

  eventPeaksDetection peaksForApneaDetection;
  welchTransform<WEL_REAL_FFT_LEN, WEL_FFT_LEN> welchObj;

  FIRFilter filterDesignForAHI;

  void DetectPeaksInfo(void);

  void EliminateOutliers(void);
  void EliminateDcOffset(void);
  void ResetAFETaskVariable(void);
  waveform PowerSpectra(void);

  void ValidateOpenAirflow(const double);

  void TherapyFlowProcess(void);
  void ExamineAirflowData(void);
  uint32_t GetEpochTimeGMT(void);
  
public:
  airFlowEvaluationTask()
      : validateBit{false}, blockNegativePeakAppend{false},
        deriveSignalWave{waveform::zero_freq_wave}, incrementCounter{0U},
        occupiedPackets{0U}, oneMinCounter{0U}, leakageRepeativeCount{0U},
        meanWindowIndex{0U}, cmfLeakRate{0.0}, meanFlowWindow{0.0},
        recFlowPacket{0U}, welchFlowInput{0U} {}
  void
  InitializeVar(); /* parasoft-suppress MISRACPP2023-13_3_1-a "Task Handler" */
  void ProcessFlowData(const float *);
  void InitializeDateTime(std::string argStoredDate);

};
#endif /* INC_AIRFLOWEVALUATIONTASK_HPP */
