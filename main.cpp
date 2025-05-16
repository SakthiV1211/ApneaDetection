#include "AirFlowEvaluationTask.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>

airFlowEvaluationTask airFlowObj;
int main()
{
  airFlowObj.InitializeVar();
  std::ifstream inputFile("ResmedFlow.txt");
  std::cout << "ReadFile!\n";
  std::cout << "Workspace directory: " << getWorkSpacePath << std::endl;
  std::array<float, SAMPLE_FRAME_LENGTH> flowRate;
  uint16_t index = 0;
  std::string dataInStr;
  bool start{true};
  if (inputFile.is_open())
  {
    while (getline(inputFile, dataInStr))
    {
      if (start == true)
      {
        std::string datetime = dataInStr;
        airFlowObj.InitializeDateTime(datetime);
        start = false;
        continue;
      }
      flowRate.at(index) = std::stof(dataInStr);
      index++;
      if (SAMPLE_FRAME_LENGTH == index)
      {
        airFlowObj.ProcessFlowData(flowRate.begin());
        index = 0u;
        flowRate.fill(0U);
      }
    }
  }
  ahiDefineBound::outFile.close();
  ahiDefineBound::debugFile.close();
  ahiDefineBound::printFile.close();
  ahiDefineBound::peakPosPrint.close();
  std::cout << "Apnea Count - " << apneaCount << ", Hypopnea Count - " << hypoApneaCount << std::endl;
  return 0;
}
