#ifndef INC_HAMPELFILETR_HPP
#define INC_HAMPELFILETR_HPP

#include <algorithm>
#include <cmath>

template <std::size_t dataSize, std::size_t winSize>
void HampelFilter(std::array<double, dataSize> &afterOutlierRem,
                  std::array<double, dataSize> &rawData,
                  const double nSigmas = 3.0)
{
  const double kFactor = 1.4826;
  for (uint16_t elem = 0; elem < dataSize; elem++)
  {
    int16_t secPathStart{elem - winSize};
    const uint16_t startIndex{std::max(static_cast<int16_t>(0), secPathStart)};
    uint16_t secPathEnd{elem + winSize + 1};
    const int16_t endIndex{
        std::min(static_cast<uint16_t>(dataSize), secPathEnd)};

    const uint16_t actualWindowSize = endIndex - startIndex;
    std::array<double, 2 * winSize + 1> windowElem{};

    uint16_t startPos = 0;
    for (uint16_t winElem = startIndex; winElem < endIndex;
         winElem++, startPos++)
    {
      windowElem.at(startPos) = rawData.at(winElem);
    }
    uint16_t bufferEndIndex = endIndex;
    if (endIndex >= 2 * winSize + 1)
    {
      bufferEndIndex = 2 * winSize + 1;
    }
    std::sort(windowElem.begin(), windowElem.begin() + bufferEndIndex);
    double median = windowElem.at(actualWindowSize / 2);

    std::array<double, 2 * winSize + 1> standardDev = {};
    for (uint16_t medianElem = 0; medianElem < bufferEndIndex; medianElem++)
    {
      standardDev.at(medianElem) = std::abs(windowElem.at(medianElem) - median);
    }
    std::sort(standardDev.begin(), standardDev.begin() + bufferEndIndex);
    double standardDeviation = standardDev.at(actualWindowSize / 2);
    double windowMad{standardDeviation * kFactor};
    if (windowMad == 0.0)
    {
      if (rawData.at(elem) != median)
      {
        afterOutlierRem.at(elem) = median;
      }
    }
    else
    {
      double zScore = std::abs(rawData.at(elem) - median) / windowMad;
      if (zScore > nSigmas)
      {
        afterOutlierRem.at(elem) = median;
      }
    }
  }
}

#endif