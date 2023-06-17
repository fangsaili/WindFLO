import matplotlib.pyplot as plt
import csv

def savedatas(path,data):
     with open(path,'w+') as f:
        write = csv.writer(f)
        write.writerows(data)

def savedata(path,data):
     with open(path,'w+') as f:
        write = csv.writer(f)
        write.writerow(data)

def plotg(grid,fits):
    plt.plot(range(len(fits)),fits)
    plt.show()

    plt.plot(grid[:,0],grid[:,1],'o')
    plt.show()
    print(min(fits))
