#ifndef INC_DATASAMPLEINC_HPP
#define INC_DATASAMPLEINC_HPP

#include <complex>

namespace dataSampleBound {
constexpr inline double PI{3.1415926536};

constexpr uint16_t SAMPLING_RATE{25U};
constexpr double SAMPLING_TIME{static_cast<double>(1U) / SAMPLING_RATE};

constexpr uint16_t SAMPLE_WIN_LEN_SECS{5U};
constexpr uint16_t SAMPLE_FRAME_LENGTH{
    static_cast<uint16_t>(static_cast<uint32_t>(SAMPLING_RATE) *
                          static_cast<uint32_t>(SAMPLE_WIN_LEN_SECS))};
constexpr uint16_t OSCAR_FRAME_LENGTH{SAMPLING_RATE};
constexpr uint16_t WELCH_FRAME_LENGTH{SAMPLE_FRAME_LENGTH * 4U};
constexpr uint16_t WELCH_FRAME_PERCENT{static_cast<uint16_t>(static_cast<double>(WELCH_FRAME_LENGTH) * 0.10)};
constexpr uint16_t HANNING_WINDOW_LENGTH{SAMPLING_RATE * 8U};
constexpr uint16_t OVERLAP_WELCH_LENGTH{SAMPLING_RATE * 4U};
constexpr uint16_t WEL_REAL_FFT_LEN{1025U};
constexpr uint16_t WEL_FFT_LEN{(WEL_REAL_FFT_LEN - 1U) * 2U};

enum class waveform : uint8_t { zero_freq_wave, sine_wave, other_freq_wave };

}; // namespace dataSampleBound
#endif /*INC_DATASAMPLEINC_HPP*/
