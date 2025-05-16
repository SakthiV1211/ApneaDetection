import math

specificGravity = 0.0013

def printFUnctionInList(printList):
    for var in printList:
        print(var)
        
def convToBar(diffPresList):
    psiList = [math.pow(10,-5)*temp for temp in diffPresList]
    return psiList

def convTom3PerHr(flowList):
    flwCmH20 = [0.06*temp for temp in flowList]
    return flwCmH20

def Average(Lst):
    return (sum(Lst)/len(Lst))

def convToFLowUsingFlowFactor(diffPrsInBar,flowFactor = 180):
    flowRate = []
    multiplyBy = 1
    for listElem in range(len(diffPrsInBar)):
        if diffPrsInBar[listElem] <= 0:
            multiplyBy = -1;
        else:
            multiplyBy = 1;
        flowRateVal = multiplyBy * flowFactor * math.sqrt(abs(diffPrsInBar[listElem])/specificGravity)
        (flowRate.append(round(flowRateVal,2)))
    return flowRate

def getFlowFactorRespectToDPandFlow(FlowCon,PrsConv):
    flowFactor = []
    for temp in range(len(FlowCon)):
        flowFactorVal = FlowCon[temp] * math.sqrt(specificGravity/PrsConv[temp])
        flowFactor.append(round(flowFactorVal,2))
    return flowFactor

def volumetricFlow(flowRate,inhaleTime):
    volume = [((rate * inhaleTime * 1000)/60) for rate in flowRate]
    return volume
    