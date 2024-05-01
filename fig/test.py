import os
import sys
import numpy as np
from pprint import pprint
import matplotlib.pyplot as plt
import pandas as pd
from qiskit import transpile
from qiskit_aer import AerSimulator
from matplotlib.patches import Circle

def plotSv(n,a,b):
    #print('iii')
    #print(len(cratioDict))
    with open('tstatevector'+str(n)+'.txt', 'r', encoding='utf-8') as file:
        text = file.readlines()

    # 将文本内容转换为NumPy数组
    cratioDict = np.array(text)
    #cratioDict = cratioDict.astype(float)
    cratioDict = list(cratioDict)

    cratioDict = cratioDict[a:b]
    plt.figure(figsize=(4,1.5))
    #x = [i for i in range(1024)]
    #nn = len(cratioDict)-1024
    #for i in range(nn,0,-1) :
    #    x.append(n-i)
    x = [i for i in range(a,b)]
    plt.plot(x, cratioDict)

    # 添加标题和标签
    #plt.title('折线图示例')
    #extra_info = "#Nodes="+str()
    extra_info = "statevector"+str(n)
    plt.title(extra_info)
    #plt.gca().invert_yaxis()
    #plt.text(5, 5,extra_info, fontsize=2, ha='right', va='top', color='blue')

    #plt.xlabel('statevector index')
    #plt.ylabel('amplitude')
    plt.tight_layout()
    plt.show()

def read(n) :
    with open(n, 'r', encoding='utf-8') as file:
        text = file.readlines()

    # 将文本内容转换为NumPy数组
    cratioDict = np.array(text)
    #print(cratioDict)
    cratioDict = cratioDict.astype(float)
    cratioDict = list(cratioDict)
    return cratioDict



def plot3():
    l1 = read("tstatevector50.txt")[:512]
    for i in range(512) :
        if(l1[i])>0 and l1[i]<0.149 :
            l1[i]+=0.2
        if l1[i]<0 :
            l1[i]-=0.2
    l2 = read("tstatevector271.txt")[:3072]
    l3 = read("tstatevector-1.txt")
    x1 = [i for i in range(512)]
    x2 = [i for i in range(3072)]
    x3 = [i for i in range(272)]
    #fig, (ax1, ax2, ax3) = plt.subplots(3,
    #                                    5,
    #                                    gridspec_kw={'left': 0.1,
    #                                                 'right': 0.9,
    #                                                 'bottom': 0.1,
    #                                                 'top': 0.9,
    #                                                'height_ratios': [1, 1, 2]})
    fig = plt.figure(figsize=(8,4))


    axmain = fig.add_axes([0.1,0.1,0.8,0.8])
    axmain.plot(x3,l3)
    axmain.set_title('The compression rate of RC')
    axmain.plot([56.8,50],[0.00465,3.43e-05],linestyle='-', color='black')
    axmain.plot([271,217.5], [0.03125,0.02690], linestyle='-', color='black')
    c1 = Circle((50,0.0000343), radius=0.0001, edgecolor='red', facecolor='blue')
    #c2 = [(0, 0, 1, 'red'), (1, 1, 0.5, 'green'), (-1, 1, 0.8, 'blue'), (-1, -1, 1.5, 'orange')]
    #axmain.add_patch(c1)
    # highlight_index = 50
    highlight_x: int = 50
    highlight_y = 0.0000343
    plt.scatter(highlight_x, highlight_y, color='blue', s=10, label='Highlighted Point')
    plt.annotate('', (highlight_x, highlight_y), textcoords="offset points",
                 xytext=(-20, 10), ha='center')
    highlight_indexx = 268
    highlight_x = 271
    highlight_y = 0.03125
    plt.scatter(highlight_x, highlight_y, color='blue', s=10, label='Highlighted Point')
    plt.annotate('', (highlight_x, highlight_y), textcoords="offset points",
                 xytext=(-20, 10), ha='center')




    ax50 = fig.add_axes([0.2,0.27,0.5,0.2])
    ax50.plot(x1,l1)
    ax50.set_title('StateVector50')
    ax50.set_yticks(ticks = [-0.2,0,0.2,0.5],labels = ['-0.02','0.0','0.02','0.5'])
    ax50.text(0.95, 0.95, '(Required RC Nodes:36)', verticalalignment='top', horizontalalignment='right',
            transform=ax50.transAxes, fontsize=10)
    #axmain.annotate('', xytext=(271,0.03125), xy=(217.5,0.02690),
    #            arrowprops=dict(facecolor='black', shrink=0.005,linewidth = 0.000001,linestyle='-'))
    #axmain.annotate('', xy=(109.2,0.00257), xytext=(50,3.43e-05),
                    #arrowprops=dict(facecolor='black', shrink=0.005,linewidth = 0.000001,linestyle='solid'))
    #                arrowprops=None)
    #plt.plot([0.00257,3.43e-05],[109.2,50], color='black', linestyle='-')
    #.plot( [0.03125,0.02690],[271,217.5], color='black', linestyle='-')




    ax271 = fig.add_axes([0.2,0.62,0.5,0.2])
    ax271.plot(x2, l2)
    ax271.set_title('StateVector271')
    ax271.set_yticks(ticks=[-0.001,0,0.01,0.015], labels=['','0.0', '0.01', ''])
    ax271.text(0.95, 0.95, '(Required RC Nodes:32768)', verticalalignment='top', horizontalalignment='right',
              transform=ax271.transAxes, fontsize=10)

    #axmain.set_position([0.1, 0.1, 0.8, 0.5])

    #plt.tight_layout()
    plt.rcParams['font.family'] = 'Times New Roman'
    plt.rcParams['font.weight'] = 'bold'


    plt.show()



def test() :
    fig, ax3 = plt.subplots()

    # 添加第一个子图到第三个子图内
    ax1 = fig.add_subplot(223)
    ax1.plot([1, 2, 3], [4, 5, 6])
    ax1.set_title('First Subplot')

    # 添加第二个子图到第三个子图内
    ax2 = fig.add_subplot(224)
    ax2.plot([1, 2, 3], [6, 5, 4])
    ax2.set_title('Second Subplot')

    # 在第三个子图中绘制内容
    ax3.plot([1, 2, 3], [8, 7, 6])
    ax3.set_title('Third Subplot')

    # 调整第三个子图的位置
    ax3.set_position([0.1, 0.1, 0.8, 0.5])  # 设置第三个子图的位置 [left, bottom, width, height]

    # 显示图形
    plt.show()
def WriteToTxt(dic,n):
    file = open("tstatevector"+str(n)+".txt","w")
        #tot = 0
    for i,x in enumerate(dic) :
        file.write(str(x))
        #tot+=x[1]
        file.write('\n')
    file.close()


def plot4():
    namefpzip = 'ratiofpzip.txt'
    namerc = 'ratioRC.txt'
    namesz = 'ratiosz.txt'
    namezfp = 'ratiozfp.txt'

    lf = read(namefpzip)
    lr = read(namerc)
    ls = read(namesz)
    lz = read(namezfp)

    x = [i for i in range(len(lz))]
    plt.plot(x, lf)
    plt.plot(x, lr)
    plt.plot(x, ls)
    plt.plot(x, lz)

    plt.show()


if __name__ == '__main__':
    #test()
    plot3()
    #plot4()
    #plotSv(50,0,512)
    #plotSv(271,0,512)
    #plotSv(-1,0,271)
    #a = read(50)
    #print((a))
    #WriteToTxt(a[:600],50)
    #a = read(271)
    #WriteToTxt(a[:3600],271)

