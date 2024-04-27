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
    compressors = ['RC', 'SZ', 'ZFP', 'FPZIP']
    cratioDict = {}
    for compressor in compressors:
        cratioDict[compressor] = []

    for i, (key, sv) in enumerate(sorted(svDict.items(), key=lambda x: int(x[0]))):
        sv = np.asarray(sv)

        # check if sv is a real state vector
        realParts = np.ascontiguousarray(np.real(sv))
        # realParts = list(realParts)
        # realParts = np.array(realParts)
        # imagParts = np.imag(sv)
        # if not np.all(np.abs(imagParts) < 1e-15):
        #     print(f'[WARNING] Imaginary parts are not all zero for state vector {key}!')

        # Calculating the compression ratios
        for compressor in compressors:
            cratio = compressionRatio(realParts, compressor)
            cratioDict[compressor].append(cratio)

    return cratioDict

def WriteToExcel(dic, fname):
    df = pd.DataFrame(dic)
    currDir = os.path.dirname(os.path.abspath(sys.argv[0]))
    df.to_excel(f'{currDir}/cratios/{fname}.xlsx', index=True, engine='openpyxl')
    return

def compressionRatio(sv, compressor):
    cratio = 0
    if compressor == 'RC':
        rc = ComData(sv)
        cratio = rc.ratio()
        # print(rc.later)
    else:
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
    elif circuitName == 'QFT':
        qc = QFT(numQubits)
    elif circuitName == 'RandomRegular':
        qc = RandomRegular(numQubits, 100)
    elif circuitName == 'RandomMedium':
        qc = RandomMedium(numQubits, 100)
    elif circuitName == 'RandomRandom':
        qc = RandomRandom(numQubits, 100)
    else:
        print('[ERROR] Undefined quantum circuit!')

    svDict = qiskitSim(qc)
    cratioDict = processSvDict(svDict)
    WriteToExcel(cratioDict, f'{circuitName}_{qc.num_qubits}')

    # print the worst compression ratio
    bestCratioDict = {key: min(value) for key, value in cratioDict.items()}
    print('The best cratio: ')
    pprint(bestCratioDict)
    print()

    worstCratioDict = {key: max(value) for key, value in cratioDict.items()}
    print('The worst cratio: ')
    pprint(worstCratioDict)
    print()

    # qc.draw('mpl') # draw the circuit
