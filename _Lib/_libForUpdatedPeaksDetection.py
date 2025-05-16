def ValleysInfo(data):
    if(data[1]<data[0] and data[1]<data[2]):
        return True
    return False

def PeaksInfo(data):
    if(data[1]>data[0] and data[1]>data[2]):
        return True
    return False

def FindPeaksAndValleyAlgo(data, minDist = 0):
    listData = []
    pkState = 'Peaks'
    peaksData = []
    peaksIndex = []
    valleysData = []
    valleysIndex = []
    functionPointers = {'Peaks':PeaksInfo,'Valleys':ValleysInfo}
    ignoreData = 0
    identifiedState = False
    lastPeak = 0
    lastPeakAxis = 0
    slopeStart = False
    localCount = 0
    slopConst = 4
    longRangeCount = minDist * 6
    for x in range(1,len(data)-1):
        if slopeStart == True:
            localCount = localCount + 1
            if localCount >= longRangeCount:
                if(pkState == 'Valleys'):
                    pkState = 'Peaks'
                    longRangeCount = minDist * 6
                    ignoreData = x
                    localCount = 0
                else:
                    peaksIndex.append(lastPeakAxis)
                    peaksData.append(lastPeak)
                    pkState = 'Peaks'
                    longRangeCount = minDist * 6
                    ignoreData = x
                    localCount = 0
                slopeStart = False
            else:
                differenceInPeak = data[x] - lastPeak
                slopeDifference = (differenceInPeak)
                # print(pkState,differenceInPeak, slopeDifference)
                if(pkState == 'Peaks'):
                    if(slopeDifference <= -(slopConst * 1.5)):
                        peaksIndex.append(lastPeakAxis)
                        peaksData.append(lastPeak)
                        pkState = 'Valleys'
                        longRangeCount = minDist * 2
                        ignoreData = x + minDist - localCount
                        localCount = 0
                        slopeStart = False
                    elif (slopeDifference >= ((slopConst*1)/2)):
                        localCount = 0
                        slopeStart = False
                else:
                    if(slopeDifference >= slopConst):
                        valleysIndex.append(lastPeakAxis)
                        valleysData.append(lastPeak)
                        pkState = 'Peaks'
                        longRangeCount = minDist * 6
                        ignoreData = x + minDist - localCount
                        # print("AfterValleyIgnoreData ",x, ignoreData, localCount)
                        localCount = 0
                        slopeStart = False
                    elif (slopeDifference <= -((slopConst*1)/2)):
                        localCount = 0
                        slopeStart = False
        else:
            if x > ignoreData:
                listData = data[x-1:x+2]
                identifiedState = functionPointers[pkState](listData)   
                if identifiedState == True:
                    lastPeak = data[x]
                    lastPeakAxis = x
                    slopeStart = True
    
    return peaksData,peaksIndex,valleysData,valleysIndex



