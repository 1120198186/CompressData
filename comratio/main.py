import os
import sys
import numpy as np
from pprint import pprint
import matplotlib.pyplot as plt
import pandas as pd
from qiskit import transpile
from qiskit_aer import AerSimulator

from Grover import Grover
from QFT import QFT
from Random import RandomRegular, RandomMedium, RandomRandom
from RC import ComData
from offline import LibPress

'''
The versions of qiskit and qiskit-aer used in this script:
>>> qiskit.__version__
'1.0.2'
>>> qiskit_aer.__version__
'0.14.0.1'
'''

def qiskitSim(qc):
    ''' Run qc using the qiskit statevector simulator '''
    simulator = AerSimulator(method='statevector')
    qc = transpile(qc, simulator) # transpile the CC...Z gate
    svDict = simulator.run(qc).result().data() # get multiple saved state vectors
    return svDict

def processSvDict(svDict):
    '''
    Check if the amplitudes in all state vectors are real numbers,
    only save the real parts (currently), and calculate the compression ratio
    '''
    compressors = ['RC', 'sz', 'zfp', 'fpzip']
    #compressors = ['RC']

    cratioDict = {}
    cratioList = []
    for compressor in compressors:
        cratioDict[compressor] = []

    for i, (key, sv) in enumerate(sorted(svDict.items(), key=lambda x: int(x[0]))):
        sv = np.asarray(sv)

        # check if sv is a real state vector
        realParts = np.ascontiguousarray(np.real(sv))
        realParts = np.round(realParts,2)
        realParts = np.where(realParts == -0.0, 0.0, realParts)
        
        #realParts = np.real(sv)
        #realParts = list(realParts)
        #realParts = np.array(realParts)
        # imagParts = np.imag(sv)
        # if not np.all(np.abs(imagParts) < 1e-15):
        #     print(f'[WARNING] Imaginary parts are not all zero for state vector {key}!')
        #if key == '0' :
        #    test(realParts)

        # Calculating the compression ratios
        for compressor in compressors:
            cratio = compressionRatio(realParts, compressor)
            cratioDict[compressor].append(cratio)
            #cratioList.append(cratio)
        #if key == '50' :
        #    cratio = compressionRatio(realParts, compressor)
            #WriteToExcel(realParts[],"SV50")
        #    WriteToTxt(realParts,50)
        #if key == str(len(svDict)-1) :
        #    cratio = compressionRatio(realParts, compressor)
            #WriteToExcel(realParts[:1024]+realParts[-1024:],"SVlast")
        #    WriteToTxt(realParts,len(svDict)-1)
    print(cratioDict)
    
    for i in compressors:
        WriteToTxt(cratioDict[i],'ratio'+i+'.txt')
    return cratioDict

def test(sv) :
    with open('example.txt', 'w') as f:
        for i in sv :
            f.write(str(i)+'\n')

def WriteToExcel(dic, fname):
    df = pd.DataFrame(dic)
    currDir = os.path.dirname(os.path.abspath(sys.argv[0]))
    df.to_excel(f'{currDir}/cratios/{fname}.xlsx', index=True, engine='openpyxl')
    return

def WriteToTxt(dic,n):
    print(n)
    file = open(n,"w")

        #tot = 0
    for i,x in enumerate(dic) :
        file.write(str(x))
        #tot+=x[1]
        file.write('\n')
    file.close()




def compressionRatio(sv, compressor,label = 0):
    #sv =  np.random.rand(10000)
    cratio = 0
    if compressor == 'RC':
        #print(sv)
        rc = ComData(sv)
        cratio = rc.ratio()
        #print(rc.later)
        # print(rc.later)
        #if(label) :
            #print(rc.later)
            #print(cratio)
            #rc.WriteToTxt(str(label))
    else:
        #print(sv)
        cratio = LibPress(sv, compressor)
    return cratio

def plotCratio(cratioDict):
    ''' Plot the compression ratios '''
    for key, cratioList in cratioDict:
        x = np.arange(len(cratioList))
        plt.plot(x, cratioList, label = key)
    plt.legend()
    plt.show()
    return


def Execute(qc) :
    svDict = qiskitSim(qc)
    cratioDict = processSvDict(svDict)
    #WriteToExcel(cratioDict, f'{circuitName}_{qc.num_qubits}')
    bestCratioDict = {key: min(value) for key, value in cratioDict.items()}
    #print('The best cratio: ')
    #pprint(bestCratioDict)
    #print()

    worstCratioDict = {key: max(value) for key, value in cratioDict.items()}
    #print('The worst cratio: ')
    #pprint(worstCratioDict)
    #print()
    #WriteToExcel(cratioDict['RC'],'compression_ratio')
    return [bestCratioDict,worstCratioDict]



def Display(temp) :
# print the worst compression ratio
    bestCratioDict = temp[0]
    worstCratioDict = temp[1]
    #bestCratioDict = {key: min(value) for key, value in cratioDict.items()}
    print('The best cratio: ')
    pprint(bestCratioDict)
    print()

    #worstCratioDict = {key: max(value) for key, value in cratioDict.items()}
    print('The worst cratio: ')
    pprint(worstCratioDict)
    print()
    return [bestCratioDict,worstCratioDict]

    # qc.draw('mpl') # draw the circuit


if __name__ == '__main__':

    circuitName, numQubits, qc = None, None, None

    if len(sys.argv) != 3:
        print('[WARNING] Usage: python main.py <circuit name> <#qubits>')
        circuitName = input('Please input the circuit name: ')
        numQubits = int(input('Please input the number of qubits: '))
    else:
        circuitName = sys.argv[1]
        numQubits   = int(sys.argv[2])
    
    if circuitName == 'Grover':
        qc = Grover(numQubits)
        Execute(qc)
        Display(Execute(qc))
    elif circuitName == 'QFT':
        qc = QFT(numQubits,2**14)
        #Execute(qc)
        Display(Execute(qc))
    elif circuitName == 'RandomRegular':
        qc = RandomRegular(numQubits, 100)
        cratiol = Execute(qc)
        for i in range(9) :
            qc = RandomRegular(numQubits, 100,0)
            temp = Execute(qc)
            for key, value in temp[0].items() :
                cratiol[0][key] += value
            for key, value in temp[1].items() :
                cratiol[1][key] += value
        for key, value in cratiol[0].items() :
            cratiol[0][key] /= 10
        for key, value in cratiol[1].items() :
            cratiol[1][key] /= 10

        Display(cratiol)


    elif circuitName == 'RandomMedium':
        qc = RandomMedium(numQubits, 100)
        cratiol = Execute(qc)
        for i in range(9) :
            qc = RandomMedium(numQubits, 100,0)
            temp = Execute(qc)
            for key, value in temp[0].items() :
                cratiol[0][key] += value
            for key, value in temp[1].items() :
                cratiol[1][key] += value
        for key, value in cratiol[0].items() :
            cratiol[0][key] /= 10
        for key, value in cratiol[1].items() :
            cratiol[1][key] /= 10

        Display(cratiol)
    elif circuitName == 'RandomRandom':
        qc = RandomRandom(numQubits, 100)
        cratiol = Execute(qc)
        for i in range(9) :
            qc = RandomRandom(numQubits, 100,0)
            temp = Execute(qc)
            for key, value in temp[0].items() :
                cratiol[0][key] += value
            for key, value in temp[1].items() :
                cratiol[1][key] += value
        for key, value in cratiol[0].items() :
            cratiol[0][key] /= 10
        for key, value in cratiol[1].items() :
            cratiol[1][key] /= 10

        Display(cratiol)
    else:
        print('[ERROR] Undefined quantum circuit!')

    
