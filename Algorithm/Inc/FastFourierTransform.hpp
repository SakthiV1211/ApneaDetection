#ifndef INC_FFT_HPP
#define INC_FFT_HPP

#include <bits/stdc++.h>
#include <algorithm>
#include <array>
#include "DataSampleInc.hpp"

using namespace dataSampleBound;

template<std::size_t nRealFFT, std::size_t nFFT>
/**
 * @class fastFourierTransform
 * @brief The class FFT.
 */
class fastFourierTransform
{
  private:
	uint16_t noOfBits;
	std::array<double, nRealFFT> frequencyBin;

	uint16_t BitReversal(uint16_t, uint16_t) const;
	void Decimation(std::array<std::complex<double>, nFFT>&,
					std::array<std::complex<double>, nFFT>&);
	void RealFFTCalculation(std::array<std::complex<double>, nRealFFT>&,
							std::array<std::complex<double>, nFFT>&);

  protected:
	void Periodogram(std::array<double, nRealFFT>&, std::array<std::complex<double>, nFFT>&);

  public:
	fastFourierTransform();

	void FFTCalculation(std::array<std::complex<double>, nFFT>&,
						std::array<std::complex<double>, nFFT>&);
	void InverseFFTCalculation(std::array<std::complex<double>, nFFT>&,
							   std::array<std::complex<double>, nFFT>&);
	std::pair<double, double> IdentifyFreqOftheSignal(std::array<double, nRealFFT>&);
	double GetFreqValueFromIndex(const uint16_t);
	double FreqPointDiff(void);
	void RealFrequencyBin(const uint16_t);
	std::pair<uint16_t, uint16_t> GetBandWidthIndex(const double, const double);
};

//! @brief Constructor of the class FFT
//! @param fs --> sampling frequency
//! @param nfft --> length of Fast Fourier Transform

template<std::size_t nRealFFT, std::size_t nFFT>
fastFourierTransform<nRealFFT, nFFT>::fastFourierTransform() : noOfBits{0U}, frequencyBin{0U}
{
	for(uint16_t index = 0U; index < 20U; index++)
	{
		if(static_cast<uint16_t>((nRealFFT - 1U) &
								 static_cast<uint16_t>(1U << static_cast<uint32_t>(index))) != 0U)
		{
			noOfBits =
				static_cast<uint16_t>(static_cast<uint32_t>(index) + static_cast<uint32_t>(1U));
			break;
		}
	}
}
//! @brief Constructor of the class FFT
//! @param fs --> sampling frequency
//! @param nfft --> length of Fast Fourier Transform
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
double fastFourierTransform<nRealFFT, nFFT>::GetFreqValueFromIndex(const uint16_t freqIndex)
{
	return frequencyBin.at(freqIndex);
}

template<std::size_t nRealFFT, std::size_t nFFT>
double fastFourierTransform<nRealFFT, nFFT>::FreqPointDiff(void)
{
	return (frequencyBin.at(1U) - frequencyBin.at(0U));
}

//! @brief Decimation In Frequency
//! @param decimate -> Hold the value after Decimation
//! @param input -> Input Samples of FFT window length
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::Decimation(
	std::array<std::complex<double>, nFFT>& decimate, std::array<std::complex<double>, nFFT>& decIn)
{
	const uint16_t fftTotLen{static_cast<uint16_t>(nFFT)};
	for(uint16_t index = 0U; index < fftTotLen; index++)
	{
		decimate.at(static_cast<std::size_t>(BitReversal(index, static_cast<uint16_t>(noOfBits)))) =
			decIn.at(index);
	}
}

//! @brief Bit reversal Function based upon number of shift value
//! @param indexVar -> value to be reversed
//! @param shift -> number of bits to be reversed
//! @return reverseVar -> reversed value

template<std::size_t nRealFFT, std::size_t nFFT>
uint16_t fastFourierTransform<nRealFFT, nFFT>::BitReversal(uint16_t indexVar, uint16_t shift) const
{
	uint16_t reverseVar{0U};
	while(shift > 0U)
	{
		const uint16_t bit{
			static_cast<uint16_t>(static_cast<uint32_t>(indexVar) & static_cast<uint32_t>(1U))};
		--shift;
		reverseVar = static_cast<uint16_t>(
			static_cast<uint32_t>(reverseVar) |
			static_cast<uint32_t>(static_cast<uint32_t>(bit) << static_cast<uint32_t>(shift)));
		indexVar =
			static_cast<uint16_t>(static_cast<uint32_t>(indexVar) >> static_cast<uint32_t>(1U));
	}
	return reverseVar;
}

//! @brief Perform FFT using Butterfly Method
//! @param fft -> Output of the FFT Algorithm contains both Real and Imag part
//! @param input -> Input Samples of FFT window length
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::FFTCalculation(
	std::array<std::complex<double>, nFFT>& fft, std::array<std::complex<double>, nFFT>& inputData)
{
	Decimation(fft, inputData);
	for(uint16_t nPointButterfly = 1U; nPointButterfly <= noOfBits; nPointButterfly++)
	{
		const uint16_t nextPairIndex{static_cast<uint16_t>(
			static_cast<uint32_t>(1U) << static_cast<uint32_t>(nPointButterfly))};
		const uint16_t pairVariable{static_cast<uint16_t>(static_cast<uint32_t>(nextPairIndex) >>
														  static_cast<uint32_t>(1U))};
		double kVar{0U};
		for(uint16_t numberOfPairs = 0U; numberOfPairs < pairVariable; numberOfPairs++)
		{
			const std::complex<double> Wn{cos(2U * kVar * PI), -sin(2U * kVar * PI)};
			uint16_t index{numberOfPairs};
			while(index < nFFT)
			{
				const uint16_t pairIndex{static_cast<uint16_t>(
					static_cast<uint32_t>(index) + static_cast<uint32_t>(pairVariable))};
				std::complex<double> firstSeg{fft.at(index) + (fft.at(pairIndex) * Wn)};
				std::complex<double> secondSeg{fft.at(index) - (fft.at(pairIndex) * Wn)};
				fft.at(index) = firstSeg;
				fft.at(pairIndex) = secondSeg;
				index = static_cast<uint16_t>(static_cast<uint32_t>(index) +
											  static_cast<uint32_t>(nextPairIndex));
			}
			kVar = (static_cast<double>((numberOfPairs + 1U)) / static_cast<double>(nextPairIndex));
		}
	}
}

//! @brief Perform FFT using Butterfly Method
//! @param fft -> Output of the FFT Algorithm contains both Real and Imag part
//! @param input -> Input Samples of FFT window length
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::InverseFFTCalculation(
	std::array<std::complex<double>, nFFT>& ifft, std::array<std::complex<double>, nFFT>& fftInput)
{
	std::array<std::complex<double>, nFFT> inverseFFTBuffer;
	const uint16_t fftTotLen{static_cast<uint16_t>(nFFT)};
	for(uint16_t inpIndex = 0U; inpIndex < fftTotLen; inpIndex++)
	{
		inverseFFTBuffer.at(inpIndex) = fftInput.at(inpIndex) / static_cast<double>(nFFT);
	}

	for(uint16_t nPointButterfly = 1U; nPointButterfly <= noOfBits; nPointButterfly++)
	{
		uint16_t segCount{1U << (nPointButterfly - 1U)};
		uint16_t nextSeg{1U << (noOfBits - nPointButterfly + 1U)};
		uint16_t pairVariable{1U << (noOfBits - nPointButterfly)};
		uint16_t startIndex{0U};

		for(uint16_t segNum = 0U; segNum < segCount; segNum++, startIndex += nextSeg)
		{
			uint16_t powerWn{0U};
			for(uint16_t index = startIndex; index < (pairVariable + startIndex);
				index++, powerWn++)
			{
				double kVar{static_cast<double>(powerWn) / nextSeg};
				std::complex<double> Wn{cos(2U * kVar * PI), sin(2U * kVar * PI)};
				std::complex<double> firstSeg{inverseFFTBuffer.at(index) +
											  (inverseFFTBuffer.at((index + pairVariable)))};
				std::complex<double> secondSeg{
					((inverseFFTBuffer.at(index) - inverseFFTBuffer.at((index + pairVariable))) *
					 Wn)};
				inverseFFTBuffer.at(index) = firstSeg;
				inverseFFTBuffer.at(index + pairVariable) = secondSeg;
			}
		}
	}
	Decimation(ifft, inverseFFTBuffer);
}

//! @brief Process Real FFT
//! @param rfft -> Output of the FFT Algorithm contains only real part
//! @param input -> Input Samples of FFT window length
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::RealFFTCalculation(
	std::array<std::complex<double>, nRealFFT>& rfft,
	std::array<std::complex<double>, nFFT>& argInputD)
{
	std::array<std::complex<double>, nFFT> fft{0U};
	FFTCalculation(fft, argInputD);
	(void)std::copy_n(fft.begin(), nRealFFT, rfft.begin());
}

//! @brief Process Periodogram for a given signal
//! @param powerSpectrum -> Holds the output of power sepctrum for a input signal
//! @param windowFrame -> Input Samples of FFT window length multiplied with window function
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::Periodogram(
	std::array<double, nRealFFT>& powerSpectrum,
	std::array<std::complex<double>, nFFT>& argHanSample)
{
	std::array<std::complex<double>, nRealFFT> realFFT{0U};
	RealFFTCalculation(realFFT, argHanSample);
	const uint16_t realTotLen{static_cast<uint16_t>(nRealFFT)};
	for(uint16_t index = 0U; index < realTotLen; index++)
	{
		const double magnitudeSpectra{
			(2U * std::abs(realFFT.at(index)) / static_cast<double>(nFFT))};
		powerSpectrum.at(index) = pow(magnitudeSpectra, 2U);
	}
}

//! @brief Provide frequency bins for a given fft window length splitted with an intreval of
//! sampling time
//! @param samplingRate input signal sampling rate
//! @return None

template<std::size_t nRealFFT, std::size_t nFFT>
void fastFourierTransform<nRealFFT, nFFT>::RealFrequencyBin(const uint16_t samplingRate)
{
	uint16_t freqIndex{0U};
	const double samplingTime{1.0 / static_cast<double>(samplingRate)};

	const uint16_t realTotLen{static_cast<uint16_t>(nRealFFT)};
	for(uint16_t index = 0U; index < realTotLen; index++)
	{
		const double divider{nFFT * samplingTime};
		frequencyBin.at(freqIndex) = static_cast<double>(index) / divider;
		freqIndex++;
	}
}

//! @brief
//! @param
//! @return

template<std::size_t nRealFFT, std::size_t nFFT>
std::pair<uint16_t, uint16_t>
	fastFourierTransform<nRealFFT, nFFT>::GetBandWidthIndex(const double startValue,
															const double endValue)
{
	uint16_t startIndex{0U};
	uint16_t endIndex{static_cast<uint16_t>(frequencyBin.size())};

	const uint16_t freqEndSize{static_cast<uint16_t>(frequencyBin.size())};
	for(uint16_t freqI = 0U; freqI < freqEndSize; freqI++)
	{
		if(frequencyBin.at(freqI) >= startValue)
		{
			startIndex = freqI;
			break;
		}
	}

	for(uint16_t freqI = 1U; freqI < freqEndSize; freqI++)
	{
		if(frequencyBin.at(freqI) >= endValue)
		{
			endIndex =
				static_cast<uint16_t>(static_cast<uint32_t>(freqI) - static_cast<uint32_t>(1U));
			break;
		}
	}
	return std::make_pair(startIndex, endIndex);
}

//! @brief
//! @param
//! @return

template<std::size_t nRealFFT, std::size_t nFFT>
std::pair<double, double> fastFourierTransform<nRealFFT, nFFT>::IdentifyFreqOftheSignal(
	std::array<double, nRealFFT>& argWelchPeriodogram)
{
	double maxPwr{0.0};
	double freqOfTheSignal{0.0};
	const uint16_t realTotLen{static_cast<uint16_t>(nRealFFT)};
	for(uint16_t index = 0U; index < realTotLen; index++)
	{
		if(argWelchPeriodogram.at(index) > maxPwr)
		{
			freqOfTheSignal = GetFreqValueFromIndex(index);
			maxPwr = argWelchPeriodogram.at(index);
		}
	}
	return std::make_pair(freqOfTheSignal, maxPwr);
}

#endif /* INC_FFT_HPP */
