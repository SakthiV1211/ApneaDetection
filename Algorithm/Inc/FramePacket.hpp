#ifndef INC_FRAMEPACKET_HPP
#define INC_FRAMEPACKET_HPP
#include <array>
#include <MathAlgo.hpp>

template<typename typeT, std::size_t sz>
struct framePacket
{
	bool frameFull;
	uint16_t frameIndex;
	std::array<typeT, sz> dataFrame;
	mathAlgo mathAlgoObj;

	framePacket() : frameFull{false}, frameIndex{0U}, dataFrame{0U}
	{
	}

	void CircularDataAppend(const typeT argInputFrameD)
	{
		if(frameIndex >= sz)
		{
			(void)std::copy(dataFrame.begin() + 1U, dataFrame.end(), dataFrame.begin());
			frameIndex--;
		}
		dataFrame.at(frameIndex) = argInputFrameD;
		frameIndex++;
	}

	void AppendData(const typeT argInputFrameD)
	{
		if(frameFull == true)
		{
			ResetFrameSeg();
		}
		dataFrame.at(frameIndex) = argInputFrameD;
		frameIndex++;
	}

	void ResetFrameSeg(void)
	{
		frameFull = false;
		frameIndex = 0U;
		dataFrame.fill(0U);
	}

	void CopyRemainingData(void)
	{
		frameFull = true;
		typeT lastData{dataFrame.at(frameIndex - 1U)};
		for(uint16_t startIndex = frameIndex; startIndex < sz; startIndex++)
		{
			dataFrame.at(startIndex) = lastData;
		}
	}

	double GetFrameMean(void)
	{
		const double meanData{mathAlgoObj.GetAverage(dataFrame.begin(), frameIndex)};
		return meanData;
	}

	double GetFrameStandardDeviation(void)
	{
		const double standardDev{mathAlgoObj.GetStandardDeviation(dataFrame.begin(), frameIndex)};
		return standardDev;
	}

	void PrintFun(void)
	{
		for(uint16_t pos = 0; pos < frameIndex; pos++)
		{
			EwPrint("%d: %.2f\n", pos, dataFrame.at(pos));
		}
	}
};

#endif /* INC_FRAMEPACKET_HPP */
