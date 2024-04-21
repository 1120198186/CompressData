import  numpy as np



class ComData :
    number = 0
    later = []
    after = []


    def __init__(this,ll) -> None:
        this.later = []
        this.after = []
        this.after = ll[::]
        #print(this.after)
        for i in ll :
            #print(type(i))
            t = np.round(i,3)
            if not this.later or this.later[-1][0]!= t :
                this.later.append([t,1])
                this.number += 1
            else :
                this.later[-1][1] += 1


    def ratio(this) :
        #print(len(this.later))
        #print(this.number)
        tt = len(this.later)*4
        #return tt/(len(this.after))
        #print(this.after)
        #print(this.later)
        #print("Compression ratio is :  ",tt/len(this.after))
        file = open("statevector3.txt","a")
        file.write('********************************************************************************\n')
        for i in this.after :
            file.write(str(i)+' ')
        file.write('\n')
        for i in this.later :
            file.write(str(i)+' ')
        #file.write(this.after)
        file.write('\n')
        #file.write(this.later)
        #file.write('\n')
        #file.write("压缩率为")
        #file.write(str(tt/len(this.after)))
        #file.write('\n')
        file.close()

        return tt / (len(this.after))


