import numpy as np



class ComData :
    number = 0
    later = []
    after = []


    def __init__(this,ll) -> None:
        this.later = []
        this.after = []
        this.after = len(ll)
        #print(this.after)
        diff_idx = np.flatnonzero(np.concatenate(([True], ll[1:] != ll[:-1], [True])))

        # 获取不同元素及其连续重复次数
        unique_values = ll[diff_idx[:-1]]
        counts = np.diff(diff_idx)

        # 创建结果二维数组
        this.later = np.column_stack((unique_values, counts))

        # 计算连续重复元素的数量
        #counts = 1 + repeats.shape[0]  # 加1是因为数组的最后一个元素不参与比较

# 输出结果
        #print("Counts of consecutive repeated elements:", counts)
        '''for i in ll :
            #print(type(i))
            t = np.round(i,2)
            if not this.later or this.later[-1][0]!= t :
                this.later.append([t,1])
                this.number += 1
            else :
                this.later[-1][1] += 1
        
        '''

    def ratio(this) :
        #print(len(this.later))
        #print(this.number)
        tt = len(this.later)*4
        # #return tt/(len(this.after))
        # #print(this.after)
        # #print(this.later)
        # #print("Compression ratio is :  ",tt/len(this.after))
        # file = open("statevector3.txt","a")
        # file.write('********************************************************************************\n')
        # for i in this.after :
        #     file.write(str(i)+' ')
        # file.write('\n')
        # for i in this.later :
        #     file.write(str(i)+' ')
        # #file.write(this.after)
        # file.write('\n')
        # #file.write(this.later)
        # #file.write('\n')
        # #file.write("压缩率为")
        # #file.write(str(tt/len(this.after)))
        # #file.write('\n')
        # file.close()

        return tt / ((this.after))


