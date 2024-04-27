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
    # print(svDict)
    processSvDict(svDict)
    return

def processSvDict(svDict):
    '''
    Check if the amplitudes in all state vectors are real numbers,
    only save the real parts (currently), and calculate the compression ratio
    '''
    # compressors = ['RC', 'SZ', 'ZFP', 'FPZIP']
    compressors = ['RC']
    cratioDict = {}
    for compressor in compressors:
        cratioDict[compressor] = []

    for i, (key, sv) in enumerate(sorted(svDict.items(), key=lambda x: int(x[0]))):
        sv = np.asarray(sv)

        # check if sv is a real state vector
        realParts = np.real(sv)
        # imagParts = np.imag(sv)
        # if not np.all(np.abs(imagParts) < 1e-15):
        #     print(f'[WARNING] Imaginary parts are not all zero for state vector {key}!')

        # Calculating the compression ratios
        for compressor in compressors:
            cratio = compressionRatio(realParts, compressor)
            cratioDict[compressor].append(cratio)
        # cratio = RepeatCounterRatio(realParts)
        # cratio = LibpressioRatio(realParts)

        # print(f'sv[{key}]: compression ratio = {cratio}')
        # print(f'sv[{key}]: {realParts}')
        # if i == len(svDict) - 1:

        #     currDir = os.path.dirname(os.path.abspath(sys.argv[0]))
        #     df = pd.DataFrame(realParts)
        #     df.to_excel(f'{currDir}/QFT.xlsx', index=True)

    # plot the compression ratios
    print(cratioDict)

    # TODO: plot the cratioDict
    # plotCratio(cratioDict)
    return

def compressionRatio(sv, compressor):
    cratio = 0
    if compressor == 'RC':
        cratio = RCRatio(sv)
    # TODO: offline compressors
    else:
        cratio = LibPress(sv, compressor)
    return cratio

def RCRatio(sv):
    ''' Calculate the compression ratio of Repeat Counter '''
    # print((sv))
    # aa = ComData(list(sv))
    rc = ComData(sv)
    cratio = rc.ratio()
    # print(aa.later)
    # print(cratio)
    return cratio

# cntt = 0

# def LibpressioRatio(sv):
#     '''Calculate the compression ratio of a given state vector with Libpressio'''
#     print(sv)
#     cratio = LibPress(sv,'sz')
#     print(cratio)
#     return cratio

    # dir = 'tempsv/'+str(cntt)+'.txt'
    # cntt+=1
    # with open(dir, 'w') as file:
    #     for ii in  sv:
    #         file.write(ii)
    #         file.write('\n')
    # return 0


def plotCratio(cratioDict):
    ''' Plot the compression ratios '''
    for key, cratioList in cratioDict:
        x = np.arange(len(cratioList))
        plt.plot(x, cratioList, label = key)
    plt.legend()
    plt.show()
    return

if __name__ == '__main__':
    # qc = Grover(3)
    # qiskitSim(qc)

    qc = QFT(10)
    qiskitSim(qc)

    # qc = RandomRegular(5, 100)
    # qiskitSim(qc)

    # qc = RandomMedium(5, 100)
    # qiskitSim(qc)

    # qc = RandomRandom(5, 100)
    # qiskitSim(qc)
    # qc.draw('mpl') # draw the circuit
