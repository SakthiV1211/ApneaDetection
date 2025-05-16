def LoggedViaSegger(L):
    filterData = L[3:-2]
    floatData = [round(float(X[3:len(X)-1]),2) for X in filterData]
    return floatData

def LoggedViaUart(L):
    floatData = [float(X) for X in L]
    return floatData

def ScriptForLog(L):
    floatData = [float(X[:-1]) for X in L]
    return floatData

def printFUnctionInList(printList):
    for var in printList:
        print(var)
  