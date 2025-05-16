#ifndef INC_AHIDEF_HPP_
#define INC_AHIDEF_HPP_

#include "FramePacket.hpp"
#include "fstream"
#include "stdio.h"
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>

#include <filesystem>

namespace fs = std::filesystem;
inline fs::path getWorkSpacePath = fs::current_path().parent_path();

namespace ahiDefineBound
{

  // Function to convert epoch timestamp to human-readable format
  inline std::string epochToHumanReadable(time_t epochTime,
                                          const char *format = "%Y-%m-%d %H:%M:%S")
  {
    std::tm *timeinfo = std::localtime(&epochTime);
    std::stringstream ss;
    ss << std::put_time(timeinfo, format);
    return ss.str();
  }

  inline fs::path specificPath = getWorkSpacePath / "ApneaDetectionAlgo" / "ComparisionFolder" / "ApneaTime.csv";
  inline std::ofstream outFile(specificPath);

  inline fs::path debugPath = getWorkSpacePath / "ApneaDetectionAlgo" / "Debug" / "Debug.txt";
  inline std::ofstream debugFile(debugPath);
  
  inline fs::path printPath = getWorkSpacePath / "ApneaDetectionAlgo" / "Debug" / "print.txt";
  inline std::ofstream printFile(printPath);

  inline fs::path peakPosPath = getWorkSpacePath / "ApneaDetectionAlgo" / "Debug" / "PeakPos.csv";
  inline std::ofstream peakPosPrint(peakPosPath);

  inline volatile uint32_t peakIndexTest{0U};
  inline std::tm currentTime;
  inline uint32_t epotchTime{0};
  inline uint16_t apneaCount{0};
  inline uint16_t hypoApneaCount{0};
  constexpr uint16_t SAMPLE_WINDOW_SECS{130U};
  constexpr uint16_t PACKET_BASELINE_WINDOW{120U};
  constexpr uint16_t PACKET_METRIC_WINDOW{10U};

  constexpr uint16_t WINDOW_SHIFT_IN_SECS{1U};

  enum class negativePeakState : uint8_t
  {
    start_append,
    stop_append,
    wait_append
  };

  enum class ahiFrame : uint8_t
  {
    fill_up,
    find_out
  };

  struct thersholdContainer
  {
    double baseCutOff;
    double examineCutOff;
  };

  struct pcInfoContainer
  {
    double previousPcData;
    double pcCompareValue;
  };

  struct windowPeaks
  {
    uint16_t metricWindow;
    uint16_t baseLineWindow;
  };

  struct window
  {
    std::array<double, SAMPLE_WINDOW_SECS> positivePeakWindow;
    std::array<double, PACKET_BASELINE_WINDOW> baseLineWindow;
    std::array<double, PACKET_METRIC_WINDOW> metricWindow;
  };

  struct countInfo
  {
    uint16_t countValueIndex;
    std::array<uint16_t, 130U> peaksArray;
  };

  enum class pcState : uint8_t
  {
    check_pc_surpassed_cut_off,
    append_five_pc_data,
    append_and_examine_pc_data,
    wait_pc_to_settle
  };

  struct pcTest
  {
    pcState pcCurrentState;
    uint8_t pcCount;
    std::array<double, 2U> pcLimRange;
  };

  struct negativeInfoContainer
  {
    bool isNegPeakFrameFull;
    negativePeakState negCheckState;
    uint16_t negSettleCounter;
    double negativePeakAvg;
    std::array<double, 20U> negativeSortedArray;
    framePacket<double, 20U> negativePeaksArray;
  };
}; // namespace ahiDefineBound
#endif
