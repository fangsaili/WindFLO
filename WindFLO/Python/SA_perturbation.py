import csv
from jpype import JArray, JDouble
import numpy as np
from time import time

# set randon seed
np.random.seed(10086)

def generate_grid(wind_scenario):
    # initial max turbins grid
    xs = np.arange(0, wind_scenario.width, 8.001*wind_scenario.R)
    ys = np.arange(0, wind_scenario.height, 8.001*wind_scenario.R)
    x = np.tile(xs,(np.size(ys),1)).reshape(np.size(xs)*np.size(ys))
    y = np.tile(ys,(np.size(xs),1)).T.reshape(np.size(xs)*np.size(ys))
    grid = np.array([x,y])
    grid = grid.T
    for obs in wind_scenario.obstacles:
        bpositions = np.multiply(
                grid <= [obs['xmax'], obs['ymax']],
                grid >= [obs['xmin'], obs['ymin']])
        bina = ~np.multiply(bpositions[:,0], bpositions[:,1])
        grid = grid[bina,:]
    return grid

# def reversal(pop):
#     length = len(pop)
#     index = np.random.randint(length)
#     if pop[index] > 0.5:
#         pop[index] = 0
#     else:
#         pop[index] = 1

#     # return pop

def reversal(pop,add=True):
    point = []
    no_point = []
    for index,p in enumerate(pop):
        if p > 0.5:
            point.append(index)
        else:
            no_point.append(index)

    np.random.shuffle(point)
    np.random.shuffle(no_point)
    if add:
        if len(no_point) == 0:
            return None
        else:
            pop[no_point[0]] = 1
    else:
        if len(point) == 0:
            return None
        else:
            pop[point[0]] = 0
    

    # return pop



def move(pop):
    point = []
    no_point = []
    for index,p in enumerate(pop):
        if p > 0.5:
            point.append(index)
        else:
            no_point.append(index)
    np.random.shuffle(point)
    np.random.shuffle(no_point)
    if len(point) == 0 or len(no_point) == 0:
        return None
    else:
        pop[point[0]],pop[no_point[0]] = pop[no_point[0]],pop[point[0]]
    # return pop

def run_sa_perturbation(grid,java_evaluator,save_fit_path,save_layout_path,best_pop=None,best_fit=1,t = 0.004,t_final = 0,alpha = 0.994, n_final = 2000):
    # set save data label
    with open(save_fit_path,'w+') as f:
        a = csv.writer(f)
        a.writerow(['index','fit','number of turbines','temparature='+str(t),'final tempareture='+str(t_final),'alpha='+str(alpha),'n='+str(n_final)])

    n = 0
    max_turbs = grid.shape[0]
    best_pops = []
    best_fits = []
    best_pop = best_pop
    best_fit = best_fit
    best_layout = None

    # if best_pop is none, random generate layout and calculate fitness
    if type(best_pop) == type(None):
        best_pop = np.zeros(max_turbs)
        bins = np.random.rand(max_turbs) > 0.5
        best_pop[:] = bins
        layout_best = grid[bins, :]
        best_pops.append(best_pop)
        best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout_best)))
    else:
        best_pops.append(best_pop)
        best_fits.append(best_fit)
        # gaussion_layout.append(best_pop)

    # cycle = 0
    # cycle_pops = []
    # cycle_fits = []
    # running 
    while t > t_final and n < n_final:
        # draw gaussion distribution and generate new layout
        # newpops = best_pop.copy()
        newpops = best_pop.copy()

        # if n%40 > 20:
        #     reversal(newpops,True)
        # elif n%40 > 10:
        #     reversal(newpops,False)
        # else:
        #     move(newpops)
        randchoose = np.random.randint(3)
        if randchoose == 0:
            reversal(newpops,True)
        elif randchoose == 1:
            reversal(newpops,False)
        else:
            move(newpops)

        best_pops.append(newpops)


        # generate layout 
        layout = grid[newpops[:] == 1,:]
        best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))

        
        maybe = 1
        if best_fits[-1] > best_fit:

            maybe = np.exp(-(best_fits[-1]-best_fit)/t)
            # maybe = np.exp(-(best_fits[-1]-best_fits[-2])/best_fits[-2]/t)

            # print(maybe)
            if maybe > np.random.rand():
            # if maybe > 2:

                best_pop = newpops
                # gaussion_layout.append(newpops)
                # 0
        else:
            # best_pop = newpops
            if best_fits[-1] < best_fit:
                best_pop = newpops
                best_fit = best_fits[-1]
                best_layout = layout
           

        # save data
        with open(save_fit_path,'a+') as f:
            a = csv.writer(f)
            a.writerow([n,best_fits[-1],len(layout),t,maybe])

        with open(save_layout_path,'w+') as f:
            text_layout = csv.writer(f)
            text_layout.writerows(best_layout)

        # change tempareture
        t = t*alpha
        n += 1

        print(n,sum(newpops),t,best_fits[-1],best_fit,randchoose,maybe)

    return best_layout,best_fits



# grid = generate_grid(wind_scenario)
# (best_fits,best_pops) = run_sa(grid,java_evaluator)
# index = best_fits.index(min(best_fits))
# bins = np.array(best_pops)[index,:] == 1
# layout = grid[bins,:]
# plotg(layout,best_fits)

