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

def add(pop,prob):
    indexs = np.array(range(len(pop)))
    np.random.shuffle(indexs)
    size = int(len(pop)*prob)
    id = 0
    for i in indexs:
        pop[i] = 1
        id += 1
        if id > size:
            break
    # return pop

def delete(pop,prob=0.04):
    indexs = np.array(range(len(pop)))
    np.random.shuffle(indexs)
    size = int(len(pop)*prob)
    id = 0
    for i in indexs:
        pop[i] = 0
        id += 1
        if id > size:
            break
    # return pop

def get_point(index,w,h):
    return index%w,index//w

def search_near_point(index,w,h,d):
    x,y = get_point(index,w,h)
    search_list = []
    for i in range(-d,d):
        search_list.append((x+d,y+i))
        search_list.append((x-d,y+i+1))
        search_list.append((x+i+1,y+d))
        search_list.append((x+i,y-d))
    final_list = []
    for a,b in search_list:
        if a < 0 or a >= w or b < 0 or b >= h:
            continue
        final_list.append(b*w+a)
    print(final_list)
    return final_list

def move(pop,prob=0.04):
    point = []
    no_point = []
    for index,p in enumerate(pop):
        if p > 0.5:
            point.append(index)
        else:
            no_point.append(index)
    np.random.shuffle(point)
    np.random.shuffle(no_point)
    size = int(len(pop)*prob)
    if len(point) < size:
        for i in point:
            pop[i] = 0
    elif len(no_point) < size:
        for i in no_point:
            pop[i] = 1
    else:
        for i in range(size):
            pop[point[i]],pop[no_point[i]] = pop[no_point[i]],pop[point[i]]
    
    # return pop


def run_sa_three(grid,java_evaluator,best_pop=None,best_fit=1,t1=0.005,t2=0.005,t3=0.005,t_final = 0.00001,alpha = 0.99994, n_final = 2000,prob=0.05,width=1,height=1):

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

    initial_fit = best_fits[0]
    initial_pop = best_pop
    # running 
    while n < n_final:

        # newpops = best_pop.copy()

        # remove
        length = len(initial_pop)
        start = np.random.randint(length)
        # repeat from start
        for i in range(start,length):
            if initial_pop[i] > 0.5:
                initial_pop[i] = 0
                layout = grid[initial_pop[:] == 1,:]
                if len(initial_pop) == 0:
                    continue
                best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))
                n+=1
                delta_f = best_fits[-1] - best_fit
                if delta_f > 0:
                    if np.random.rand(1) > np.exp(delta_f/t1):
                        initial_pop[i] = 1
                    else:
                        initial_fit = best_fits[-1]
                else:
                    best_fit = best_fits[-1]
                    initial_fit = best_fits[-1]
            
            print(n,sum(initial_pop),'T_remove',t1,best_fits[-1],best_fit)
        
        # Move
        move_index = []
        for index,p in enumerate(initial_pop):
            if p > 0.5:
                move_index.append(index)
            
        for index in move_index:
            near_points = search_near_point(index,width,height,1)
            if near_points == []:
                continue
            near_has_points = []
            for point in near_points:
                if point < 0 or point >= len(initial_pop):
                    continue
                if initial_pop[point] < 0.5:
                    near_has_points.append(point)
            if near_has_points == []:
                continue
            near_has_points_fits = []
            initial_pop[index] = 0
            for p in near_has_points:
                # initial_pop[index] = 0
                initial_pop[p] = 1
                layout = grid[initial_pop[:] == 1,:]
                best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))
                n+=1
                near_has_points_fits.append(best_fits[-1])
                initial_pop[p] = 0
            initial_pop[index] = 1
            best_choose = min(near_has_points_fits)
            best_choose_index = near_has_points[near_has_points_fits.index(best_choose)]
            delta_f = best_choose - best_fit
            if delta_f > 0:
                if np.random.rand(1) < np.exp(delta_f/t2):
                    initial_pop[index] = 0
                    initial_pop[best_choose_index] = 1
                    initial_fit = best_choose
            else:
                initial_pop[index] = 0
                initial_pop[best_choose_index] = 1
                best_fit = best_choose
                initial_fit = best_choose
        
            print(n,sum(initial_pop),'T_move',t2,best_choose,best_fit)

        # add
        max_length = len(initial_pop)
        for i in range(max_length):
            if initial_pop[i] < 0.5:
                initial_pop[i] = 1
                layout = grid[initial_pop[:] == 1,:]
                best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))
                delta_f = best_fits[-1] - best_fit
                if delta_f > 0:
                    if np.random.rand(1) < np.exp(delta_f/t3):
                        initial_fit = best_fits[-1]
                    else:
                        initial_pop[i] = 0
                else:
                    best_fit = best_fits[-1]
                    initial_fit = best_fits[-1]
                n += 1
            print(n,sum(initial_pop),'T_add',t3,best_fits[-1],best_fit)
        
        t1 = alpha*t1
        t2 = alpha*t2
        t3 = alpha*t3

    best_layout = grid[initial_pop[:] == 1,:]

    return best_layout,best_fits



# grid = generate_grid(wind_scenario)
# (best_fits,best_pops) = run_sa(grid,java_evaluator)
# index = best_fits.index(min(best_fits))
# bins = np.array(best_pops)[index,:] == 1
# layout = grid[bins,:]
# plotg(layout,best_fits)

