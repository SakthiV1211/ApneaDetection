def ValleysInfo(data):
    if(data[1]<data[0] and data[1]<data[2]):
        return 'Peaks'
    return 'Valleys'

def PeaksInfo(data):
    if(data[1]>data[0] and data[1]>data[2]):
        return 'Valleys'
    return 'Peaks'

def AmplitudeOfPeaks(Peaks,Valleys):
    Amp = []
    for I in range(len(Peaks)):
        Amp.append(Peaks[I] - Valleys[I])
        # print(Amp)
    return Amp

def FindPeaksAndValleyAlgo(data, minDist = 0):
    listData = []
    var = 'Peaks'
    backUp = var
    peaksData = []
    peaksIndex = []
    valleysData = []
    valleysIndex = []
    functionPointers = {'Peaks':PeaksInfo,'Valleys':ValleysInfo}
    ignoreData = 0
    for x in range(1,len(data)-1):
        if x > ignoreData:
            listData = data[x-1:x+2]
            var = functionPointers[var](listData)                                                                                                       
            
        if(backUp != var):
            if(var != 'Peaks'):
                peaksIndex.append(x)
                peaksData.append(data[x])
            else:
                valleysIndex.append(x)
                valleysData.append(data[x])
            backUp = var
            ignoreData = x + minDist
    
    return peaksData,peaksIndex,valleysData,valleysIndex

def FindPeaks(data, minDist = 0):
    listData = []
    var = 'Peaks'
    backUp = var
    peaksData = []
    functionPointers = {'Peaks':PeaksInfo,'Valleys':ValleysInfo}
    ignoreData = 0
    for x in range(1,len(data)-1):
        if x > ignoreData:
            listData = data[x-1:x+2]
            var = functionPointers[var](listData)                                                                                                       
            
        if(backUp != var):
            if(var != 'Peaks'):
                peaksData.append(data[x])
            backUp = var
            ignoreData = x + minDist
    return peaksData

def FindValleys(data, minDist = 0):
    listData = []
    var = 'Peaks'
    backUp = var
    valleysData = []
    functionPointers = {'Peaks':PeaksInfo,'Valleys':ValleysInfo}
    ignoreData = 0
    for x in range(1,len(data)-1):
        if x > ignoreData:
            listData = data[x-1:x+2]
            var = functionPointers[var](listData)                                                                                                       
            
        if(backUp != var):
            if(var != 'Valleys'):
                valleysData.append(data[x])
            backUp = var
            ignoreData = x + minDist
    return valleysData


def UpdatedFindPeaksAndValleyAlgo(data, minDist = 0):
    listData = []
    var = 'Peaks'
    backUp = var
    peaksData = []
    peaksIndex = []
    valleysData = []
    valleysIndex = []
    functionPointers = {'Peaks':PeaksInfo,'Valleys':ValleysInfo}
    ignoreData = 0
    storedPeak = 0
    storedTime = 0
    for x in range(1,len(data)-1):
        if x > ignoreData:
            listData = data[x-1:x+2]
            var = functionPointers[var](listData)                                                                                                       
            
        if(backUp != var):
            if(var != 'Peaks'):
                peaksIndex.append(x)
                peaksData.append(data[x])
            else:
                valleysIndex.append(x)
                valleysData.append(data[x])
            backUp = var
            ignoreData = x + minDist
    
    return peaksData,peaksIndex,valleysData,valleysIndex