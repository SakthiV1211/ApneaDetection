#ifndef INC_FINDINGPEAKSINCLUDE_HPP_
#define INC_FINDINGPEAKSINCLUDE_HPP_

#include <array>
#include <stdint.h>

namespace peakDefine
{
enum class extendedSecsState
{
	zero_sec,
	one_sec,
	two_secs,
	three_secs
};

enum class peakType : uint8_t
{
	is_none,
	positive_peak,
	negative_peak
};

enum class detectionAlgorithmState : uint8_t
{
	wait_for_500ms,
	find_peak_algo,
	peak_confirm_algo
};

enum class detectState : int8_t
{
	peak_detect,
	valley_detect
};

struct peakDetectContainers
{
	detectState goToNextDetectState;
	detectionAlgorithmState algorithmState;
	uint16_t peakToValleyIntrevalGap;
};

struct peakValues
{
	double peakData;
	double valleyData;
};

struct flowRatePeakInfo
{
	uint16_t peakIndex;
	peakValues peaksValueInfo;
};

struct volumeContainer
{
	uint16_t dxInc;
	uint16_t zeroVolumeCount;
	double flowVolume;
	double filteredVolume;
};

struct filterContent
{
	double filteredFlowRateForFlex;
};

enum class breathingPattern : uint8_t
{
	inhale_pause,
	exhale_onset,
	inhale_onset,
	exhale_pause,
	idle_flow_state
};

}; // namespace peakDefine
#endif /* INC_FINDINGPEAKSINCLUDE_HPP_ */
