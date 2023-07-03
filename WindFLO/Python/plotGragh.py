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




def plotsingle(model='newAL/angle',save=''):
    load_path = 'data/%s/'%model
    save_path = 'data/%s/'%model

    # draw fitness graph
    for  i in range(1,2):
        all_fits = []
        with open(load_path+'all_fits%s.csv'%i) as f:
            temp = f.readline().split(',')
            all_fits = [float(t) for t in temp]
        # fits_index = range(len(all_fits))
        # fits = all_fits
        fits_index = []
        fits = []
        j = 0
        while j < len(all_fits):
            fits.append(min(all_fits[j:j+31]))
            fits_index.append(j+16)
            j+=31

        # print(all_fits)
        plt.figure(figsize=(32,8))
        # plt.plot(range(len(all_fits)),all_fits,label='GA'+str(min(all_fits)),color='red',linestyle='-',marker='*',linewidth=0.5)
        plt.plot(fits_index,fits,label='GA'+str(min(all_fits)),color='red',linestyle='-',marker='*',linewidth=0.5)

        plt.title('scenorio%s best fit:'%i+str(min(all_fits)))
        plt.xlabel('times')
        plt.ylabel('fitness')
        plt.legend()

        # plt.show()
        print(min(all_fits))
        plt.savefig(save_path+save+'fits_fig%s'%i)
        plt.close()

def plot_mix(models=['ga','SA']):
     # draw fitness graph
    
    for  i in range(5):
        plt.figure(figsize=(16,8))

        for model in models:
            load_path = 'data/%s/'%model
            save_path = 'data/%s/'%('_'.join(models))
    
            all_fits = []
            with open(load_path+'all_fits%s.csv'%i) as f:
                temp = f.readline().split(',')
                all_fits = [float(t) for t in temp]
        
            # print(all_fits)
            fits = []
            fits_index = []
            j = 0
            while j < len(all_fits):
                fits.append(min(all_fits[j:j+20]))
                fits_index.append(j+10)
                j+=20

            plt.plot(fits_index,fits,label=model+':'+str(min(all_fits)),linestyle='-',marker='*',linewidth=0.5)
            plt.title('scenorio%s best fit:'%i+str(min(all_fits)))
            plt.xlabel('times')
            plt.ylabel('fitness')
            plt.legend()

        # plt.show()
        print(min(all_fits))
        plt.savefig(save_path+'_'.join(models)+'_fits_mix_fig%s'%i)
        plt.close()

def plot_mix_in_one(models=['ga','SA']):
    save_path = 'data/%s/'%('_'.join(models))

     # draw fitness graph
    # plt.figure(5,figsize=(40,40))

    n = 1
    for  i in range(5):
        # plt.figure(figsize=(16,8))
        plt.figure(1,figsize=(60,30))

        for index,model in enumerate(models):
            load_path = 'data/%s/'%model

            all_fits = []
            with open(load_path+'all_fits%s.csv'%i) as f:
                temp = f.readline().split(',')
                all_fits = [float(t) for t in temp]
        
            # print(all_fits)
            
            plt.subplot(2*5,len(models),n)
            n+=1
            plt.plot(range(len(all_fits)),all_fits,label=model+':'+str(min(all_fits)),linestyle='-',marker='*',linewidth=0.5)
            plt.title('scenorio%s best fit:'%i+str(min(all_fits)))
            plt.xlabel('times')
            plt.ylabel('fitness')
            plt.legend()

            plt.subplot(2*5,1,(i+1)+5)
            plt.plot(range(len(all_fits)),all_fits,label=model+':'+str(min(all_fits)),linestyle='-',marker='*',linewidth=0.5)
            plt.title('scenorio%s best fit:'%i)
            plt.xlabel('times')
            plt.ylabel('fitness')
            plt.legend()
        # n+=1

        # plt.show()
    print(min(all_fits))
    plt.savefig(save_path+'_'.join(models)+'_fits_mix_fig_in_one')
    plt.close()


def plot_layout(model='newSA',save=''):
    load_path = 'data/%s/'%model
    save_path = 'data/%s/'%model
    for  i in range(0,5):
            
        layoutx = []
        layouty = []
        with open(load_path+'best_layout%s.csv'%i) as f:
            for l in f.readlines():
                
                point = l.split(',')
                layoutx.append(float(point[0]))
                layouty.append(float(point[1]))
        # plt.figure(figsize=1)
        plt.gca().set_aspect(1)
        plt.plot(layoutx,layouty,'.',scalex=1,scaley=1)
        plt.savefig(save_path+save+'newSA%s'%i)
        plt.close()


plot_layout(model='newSA',save='005')

# plotsingle(model='newSA',save='005')
# plot_mix(models=['ga','newSA'])
# plot_mix_in_one()
# plot_layout()



