from jpype import JArray, JDouble
import numpy as np
from time import time

import csv

# sudocode
# 1 Initialization of temperate T0 and initial guess x (0) ;
# 2 Set the final temperature Tf and the max number of iterations N;
# 3 Define the cooling schedule T→αT,(0<α<1);
# 4 whileT>Tf and t<Ndo
# 5     Move randomly to a new location: move, add or delete turbines .set change prob 
# 6     Calculate f =  ft-best_f
# 7     Accept the new solution if better;
# 8         if not improved then
# 9             Generate a random number r;
# 10            Accept if p = exp[−f/T ] > r;
# 11        end
# 12    Update the best x∗ and f∗;
# 13    t = t + 1;
# 15 end



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

def reversal(pop):
    length = len(pop)
    index = np.random.randint(length)
    if pop[index] > 0.5:
        pop[index] = 0
    else:
        pop[index] = 1

    # return pop


# def move(pop):
#     point = []
#     no_point = []
#     for index,p in enumerate(pop):
#         if p > 0.5:
#             point.append(index)
#         else:
#             no_point.append(index)
#     np.random.shuffle(point)
#     np.random.shuffle(no_point)
#     if len(point) == 0 or len(no_point) == 0:
#         return None
#     else:
#         pop[point[0]],pop[no_point[0]] = pop[no_point[0]],pop[point[0]]
    # return pop



def run_ta_standard1(grid,java_evaluator,best_pop=None,best_fit=1,t = 0.1,alpha = 0.95, n_final = 100000,cycle_limit=1000):

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
        bins = np.random.rand(max_turbs) > 0.01
        best_pop[:] = bins
        layout_best = grid[bins, :]
        best_pops.append(best_pop)
        best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout_best)))
    else:
        best_pops.append(best_pop)
        best_fits.append(best_fit)
        # gaussion_layout.append(best_pop)

    t = best_fits[0]*0.0000001

    cycle = 0
    # running 
    while n < n_final:
        # draw gaussion distribution and generate new layout
        # newpops = best_pop.copy()
        newpops = best_pop.copy()

        reversal(newpops)

        # generate layout 
        layout = grid[newpops[:] == 1,:]
        best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))

        if best_fits[-1] < best_fit + t:
            with open('test_TA_standard1_layout_100000.csv','w+') as f:
                a = csv.writer(f)
                a.writerows(layout)
            
            if best_fits[-1] < best_fit:
                best_fit = best_fits[-1]
                best_layout = layout

            best_pop = newpops
            
           
        with open('test_TA_standard1_fits_100000.csv','a+') as f:
            f.write(str(best_fits[-1])+',')

        if cycle == cycle_limit:
            t = t*alpha
            cycle = 0
        cycle += 1
        n += 1
        print(n,cycle,sum(newpops),t,best_fits[-1],best_fit)

    return best_layout,best_fits



# grid = generate_grid(wind_scenario)
# (best_fits,best_pops) = run_sa(grid,java_evaluator)
# index = best_fits.index(min(best_fits))
# bins = np.array(best_pops)[index,:] == 1
# layout = grid[bins,:]
# plotg(layout,best_fits)
