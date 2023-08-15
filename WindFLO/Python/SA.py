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

# def gaussion_prob(data,x):
#     avg = np.average(data)
#     sigma = np.std(data)
#     res = 1/np.power(2*np.pi*sigma,0.5)*np.exp(-np.power(x-avg,2)/(2*np.power(sigma,2)))
#     return res


def reversal(pop):
    length = len(pop)
    index = np.random.randint(length)
    if pop[index] > 0.5:
        pop[index] = 0
    else:
        pop[index] = 1

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

def run_sa(grid,java_evaluator,best_pop=None,best_fit=1,t = 0.004,t_final = 0.00001,alpha = 0.99, n_final = 2000):

    n = 0
    max_turbs = grid.shape[0]
    best_pops = []
    best_fits = []
    best_pop = best_pop
    best_fit = best_fit
    best_layout = None

    # set initial guassion layout
    # gaussion_layout = []
    # gaussion_layout.append(np.array([0.4999]*max_turbs))
    # gaussion_layout.append(np.array([0.5001]*max_turbs))
    # # gaussion_layout.append(np.ones(max_turbs))
    # # gaussion_layout.append(np.zeros(max_turbs))

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

    cycle = 0
    cycle_pops = []
    cycle_fits = []
    # running 
    while t > t_final and n < n_final:
        # draw gaussion distribution and generate new layout
        # newpops = best_pop.copy()
        newpops = best_pop.copy()

        opposite = -best_pop + 1
        # g_layout = np.array(gaussion_layout) 
        mutate = np.random.rand(max_turbs) < 0.03
        newpops[mutate] = opposite[mutate]
        best_pops.append(newpops)


        # generate layout 
        layout = grid[newpops[:] == 1,:]
        best_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(layout)))

        cycle_pops.append(newpops)
        cycle_fits.append(best_fits[-1])

       # whether or not a change is accepted
        # if best_fits[-1] > best_fit:
        maybe = 1
        # if best_fits[-1] > best_fits[-2]:
        if best_fits[-1] > best_fit:

            maybe = np.exp(-(best_fits[-1]-best_fit)/best_fit/t)
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
            # gaussion_layout.append(newpops)
        # gaussion_layout.append(newpops)

        # gaussion_layout.append(newpops)
        cycle += 1
        if cycle == 20:
            b = min(cycle_fits)
            best_pop = cycle_pops[cycle_fits.index(b)]
            t = t*alpha
            cycle = 0
            cycle_fits = []
            cycle_pops = []
        n += 1
        print(n,sum(newpops),t,best_fits[-1],best_fit,maybe)

    return best_layout,best_fits



# grid = generate_grid(wind_scenario)
# (best_fits,best_pops) = run_sa(grid,java_evaluator)
# index = best_fits.index(min(best_fits))
# bins = np.array(best_pops)[index,:] == 1
# layout = grid[bins,:]
# plotg(layout,best_fits)

