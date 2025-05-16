import numpy as np
from _Lib import _libLogDataConv as convToFloat
import csv

def skipFirst(pathArg):
    firstString = ""
    with open(pathArg, 'r') as file:
        # Skip the first line
        firstString = file.readline().strip()
        print("ReadFF ", firstString)
        next(file)
        # Read the remaining lines
        lines = [line.strip() for line in file]
        
    return firstString, lines

def ReadDataFromPath(path,segger=False, denom = 1):
    firstStringArg, logFile = skipFirst(path)
    # fileData = logFile.readlines()
    # logFile.close()
    den = denom
    if segger == True:
        readDataInList = convToFloat.LoggedViaSegger(logFile)
    else:
        readDataInList = convToFloat.LoggedViaUart(logFile)
        
    convertToArray = np.array(readDataInList)/den
    return firstStringArg, convertToArray

def ReadDataPath(path,segger=False, denom = 1):
    logFile = open(path,"r")
    fileData = logFile.readlines()
    logFile.close()
    den = denom
    if segger == True:
        readDataInList = convToFloat.LoggedViaSegger(fileData)
    else:
        readDataInList = convToFloat.LoggedViaUart(fileData)
        
    convertToArray = np.array(readDataInList)/den
    return convertToArray

def ReadFromCSV(fileName, column=1):
    with open(fileName,'r') as fileObj:
        # heading = next(fileObj)
        readCsv = csv.reader(fileObj)
        rCsv = []
        for elem in readCsv:
            # print(elem)
            rowD = []
            for index in range(column):
                rowD.append(float(elem[index]))
            rCsv.append(rowD)
    return rCsv      

def ReadEventFromCSV(fileName, column=1, ignoreFirst = False):
    with open(fileName,'r') as fileObj:
        if ignoreFirst == True:
            heading = next(fileObj)
        readCsv = csv.reader(fileObj)
        rCsv = []
        for elem in readCsv:
            # print(elem)
            rowD = []
            for index in range(column):
                rowD.append((elem[index]))
            rCsv.append(rowD)
    return rCsv      
