#ifndef INC_WELCHTRANSFORM_HPP
#define INC_WELCHTRANSFORM_HPP

#include <bits/stdc++.h>
#include <algorithm>
#include <array>
#include "FastFourierTransform.hpp"
#include "MathAlgo.hpp"
#include "WindowFunction.hpp"
/**
 * @class waveletTransform
 * @brief The class waveletTransform.
 */
template<std::size_t lenRealFFT, std::size_t lenFFT>
class welchTransform : public fastFourierTransform<lenRealFFT, lenFFT>
{
  private:
	double freqDx;
	std::pair<uint16_t, uint16_t> deltaBandIndex;
	std::array<double, lenRealFFT> welchPeriodogram;
	void CummulatePowerSpectra(std::array<double, lenRealFFT>& aggregateSpectra,
							   std::array<double, lenRealFFT>& powerSectra) const;
	void AveragePowerSpectra(std::array<double, lenRealFFT>& avgPwrSpectra,
							 std::array<double, lenRealFFT>& cummulativeSpectra,
							 const uint16_t numOfFrames) const;
	void DensityConvertion(std::array<double, lenRealFFT>&);
	double RelativeCutOffCalc(const double);

  public:
	welchTransform();
	void WelchTransformInit(void);
	waveform DetectFlowStatus(void);
	template<std::size_t sampleWindowLength>
	void ComputeWelchTransform(const std::array<double, sampleWindowLength>& input,
							   const uint16_t hanLen, const uint16_t argOverLapLen,
							   const bool scaling);
};

//! @brief Constructor of the class FFT
//! @param fs --> sampling frequency
//! @param nfft --> length of Fast Fourier Transform

template<std::size_t lenRealFFT, std::size_t lenFFT>
welchTransform<lenRealFFT, lenFFT>::welchTransform() :
	fastFourierTransform<lenRealFFT, lenFFT>{}, freqDx{0U}, deltaBandIndex{0U, 0U},
	welchPeriodogram{0U}
{
}

//! @brief
//! @param
//! @retval

template<std::size_t lenRealFFT, std::size_t lenFFT>
void welchTransform<lenRealFFT, lenFFT>::WelchTransformInit(void)
{
	fastFourierTransform<lenRealFFT, lenFFT>::RealFrequencyBin(SAMPLING_RATE);
	freqDx = fastFourierTransform<lenRealFFT, lenFFT>::FreqPointDiff();
	deltaBandIndex = fastFourierTransform<lenRealFFT, lenFFT>::GetBandWidthIndex(0.01, 0.8);
}

//! @brief Add the Power Spectra for every frame of welch period
//! @param aggregateSpectra -> cummulate the power spectrum
//! @param powerSectra -> power spectrum for a frame segment
//! @retval None

template<std::size_t lenRealFFT, std::size_t lenFFT>
void welchTransform<lenRealFFT, lenFFT>::CummulatePowerSpectra(
	std::array<double, lenRealFFT>& aggregateSpectra,
	std::array<double, lenRealFFT>& powerSectra) const
{
	const uint16_t endLen{static_cast<uint16_t>(lenRealFFT)};
	for(uint16_t index = 0U; index < endLen; index++)
	{
		aggregateSpectra.at(index) = aggregateSpectra.at(index) + powerSectra.at(index);
	}
}

//! @brief Process to average the power spectrum segments for a Signal Period
//! @param avgPwrSpectra -> Output contains average power spectrum
//! @param cummulativeSpectra -> Cummulative Power of splitted segments
//! @param numOfFrames -> number of frames for a single period
//! @retval None

template<std::size_t lenRealFFT, std::size_t lenFFT>
void welchTransform<lenRealFFT, lenFFT>::AveragePowerSpectra(
	std::array<double, lenRealFFT>& avgPwrSpectra,
	std::array<double, lenRealFFT>& cummulativeSpectra, const uint16_t numOfFrames) const
{
	const uint16_t endLen{static_cast<uint16_t>(lenRealFFT)};
	for(uint16_t index = 0U; index < endLen; index++)
	{
		if(numOfFrames != 0U)
		{
			avgPwrSpectra.at(index) =
				cummulativeSpectra.at(index) / static_cast<double>(numOfFrames);
		}
	}
}

//! @brief Welch's method(also called the periodogram method) for estimating power spectra is
//! carried
//!        out by dividing the time signal into successive blocks, forming the periodogram for
//!        each block, and averaging.
//! @param welchSpectra -> Output of the Welch Spectrum
//! @param input -> Input Samples
//! @param window -> Window Signals(Hanning Window)
//! @retval None

template<std::size_t lenRealFFT, std::size_t lenFFT>
template<std::size_t sampleWindowLength>
void welchTransform<lenRealFFT, lenFFT>::ComputeWelchTransform(
	const std::array<double, sampleWindowLength>& input, const uint16_t hanLen,
	const uint16_t argOverLapLen, const bool scaling)
{
	std::array<double, lenRealFFT> cummulativeSpectra{0U};
	std::array<double, lenRealFFT> powerSpectra{0U};
	welchPeriodogram.fill(0U);
	uint16_t numOfFrames{0U};
	windowFunction windowObj;
	uint16_t startWindow{0U};
	while(startWindow <= (sampleWindowLength - hanLen))
	{
		std::array<std::complex<double>, lenFFT> windowedSignal{0U};
		(void)std::copy_n(input.begin() + startWindow, hanLen, windowedSignal.begin());
		++numOfFrames;
		windowObj.HanningWindow(windowedSignal, hanLen);
		fastFourierTransform<lenRealFFT, lenFFT>::Periodogram(powerSpectra, windowedSignal);
		if(scaling == true)
		{
			DensityConvertion(powerSpectra);
		}
		CummulatePowerSpectra(cummulativeSpectra, powerSpectra);
		startWindow = static_cast<uint16_t>(static_cast<uint32_t>(startWindow) +
											static_cast<uint32_t>(argOverLapLen));
	}
	AveragePowerSpectra(welchPeriodogram, cummulativeSpectra, numOfFrames);
}

//! @brief
//! @param
//! @param
//! @retval None

template<std::size_t lenRealFFT, std::size_t lenFFT>
void welchTransform<lenRealFFT, lenFFT>::DensityConvertion(
	std::array<double, lenRealFFT>& argPowerSpectra)
{
	const double densityConstant{(SAMPLING_TIME * lenFFT) / 1.5};
	const uint16_t endLen{static_cast<uint16_t>(lenRealFFT)};
	for(uint16_t index = 0U; index < endLen; index++)
	{
		argPowerSpectra.at(index) = argPowerSpectra.at(index) * densityConstant;
	}
}

//! @brief
//! @param
//! @retval
template<std::size_t lenRealFFT, std::size_t lenFFT>
waveform welchTransform<lenRealFFT, lenFFT>::DetectFlowStatus(void)
{
	mathAlgo mathAlgoObj{};
	std::pair<double, double> getFreqInfo{
		fastFourierTransform<lenRealFFT, lenFFT>::IdentifyFreqOftheSignal(welchPeriodogram)};
	const uint16_t totalWelLen{static_cast<uint16_t>(
		static_cast<uint32_t>(welchPeriodogram.size()) - static_cast<uint32_t>(1U))};
	const double totalPower{
		mathAlgoObj.SimpsonIntegration(welchPeriodogram.begin(), totalWelLen, freqDx)};
	const double deltaPower{mathAlgoObj.SimpsonIntegration(
		welchPeriodogram.begin() + deltaBandIndex.first, deltaBandIndex.second, freqDx)};
	double relativePower{(totalPower - deltaPower) / totalPower};
	const double cutOffCoeff{RelativeCutOffCalc(totalPower)};
	if(relativePower < cutOffCoeff)
	{
		if((getFreqInfo.second < 10.0) || (getFreqInfo.first > 1.0))
		{
			return waveform::zero_freq_wave;
		}
		return waveform::sine_wave;
	}
	else
	{
		if(totalPower > 5)
		{
			return waveform::other_freq_wave;
		}
	}
	return waveform::zero_freq_wave;
}

//! @brief
//! @param None.
//! @retval None.

template<std::size_t lenRealFFT, std::size_t lenFFT>
double welchTransform<lenRealFFT, lenFFT>::RelativeCutOffCalc(const double argTotalPwr)
{
	const double minPower{4.0};
	const double aprxMaxPwr{30.0};
	const std::array<double, 2U> relativeCutOffRange{0.26, 0.22};
	if((argTotalPwr > minPower) && (argTotalPwr < aprxMaxPwr))
	{
		double xDiff{aprxMaxPwr - minPower};
		double yDiff{relativeCutOffRange.at(1U) - relativeCutOffRange.at(0U)};
		double relativeCutOff{
			(relativeCutOffRange.at(0U) + (((argTotalPwr - minPower) * yDiff) / xDiff))};
		return relativeCutOff;
	}
	else if(argTotalPwr >= aprxMaxPwr)
	{
		return relativeCutOffRange.at(1);
	}
	else
	{
	}
	return relativeCutOffRange.at(0);
}

#endif /* INC_WELCHTRANSFORM_HPP */
