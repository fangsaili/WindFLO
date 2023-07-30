import numpy as np
from KusiakEvaluator import WindScenario, WindFarmLayoutEvaluator

from jpype import JArray, JDouble
import numpy as np
from time import time

import csv
# from plotGragh import plotg,savedata,savedatas

np.random.seed(520)

num_pop = 20       # populations
tour_size = 4      # tournament size
mut_rate = 0.05   # mutation rate
cross_rate = 0.40 # crossover rate - uniform crossover
max_evals = 100000

# all_fit = []

def run_ga(wind_scenario,java_evaluator):
    # text_fits = open('test_ga_fits_100000.csv','w+')
    # text_fits = csv.writer(a)
    # a  = open('test_ga_layout_100000.csv','w+')
    # text_layout = csv.writer(a)

    all_fit = []

    xs = np.arange(0, wind_scenario.width, 8.001*wind_scenario.R)
    ys = np.arange(0, wind_scenario.height, 8.001*wind_scenario.R)
    x = np.tile(xs,(np.size(ys),1)).reshape(np.size(xs)*np.size(ys))
    y = np.tile(ys,(np.size(xs),1)).T.reshape(np.size(xs)*np.size(ys))
    grid = np.array([
        x,
        y
        ])
    grid = grid.T
    for obs in wind_scenario.obstacles:
        bpositions = np.multiply(
                grid <= [obs['xmax'], obs['ymax']],
                grid >= [obs['xmin'], obs['ymin']])
        bina = ~np.multiply(bpositions[:,0], bpositions[:,1])
        grid = grid[bina,:]
    max_turbs = grid.shape[0]
    evals = int(max_evals/num_pop)

    pops = np.zeros([max_turbs, num_pop])
    fits = np.ones(num_pop)
    best_fit = 1
    best_layout = None




    for p in range(num_pop):
        bins = np.random.rand(max_turbs) > 0.5
        pops[:,p] = bins
        layout = grid[bins, :]
        fits[p] = java_evaluator.evaluate(JArray((JArray)(JDouble))(layout))

        if fits[p] < best_fit:
            best_layout = layout
            best_fit = fits[p]

    for i in range(1,evals):
        run_time = time()
        pops_copy = pops.copy()

        # rank
        # winners = np.zeros(int(num_pop/tour_size))
        winners = [0]*int(num_pop/tour_size)

        competitors = np.arange(num_pop)
        np.random.shuffle(competitors)
        competitors = np.reshape(competitors, [len(winners), tour_size])
        for t in range(len(winners)):
            winners[t] = competitors[t,np.argmin(fits[competitors[t,:]])]

        # crossover
        children = np.zeros([max_turbs, num_pop])
        for c in range(len(winners), num_pop):
            np.random.shuffle(winners)
            parents = winners[0:2]
            child = pops[:, parents[0]]
            crossed = np.random.rand(max_turbs) < cross_rate
            child[crossed] = pops[crossed,parents[1]]
            children[:,c] = child


        # mutate
        opposite = -children + 1
        mutate = np.random.rand(max_turbs, num_pop) < mut_rate
        children[mutate] = opposite[mutate]

        # elitism
        for c in range(len(winners)):
            children[:,c] = pops_copy[:,winners[c]]

        # evaluate
        pops = children
        fits = np.ones(num_pop)

        for p in range(num_pop):
            bins = pops[:,p] == 1
            layout = grid[bins,:]
            
            # fits[p] = wfle.evaluate(layout)
            # t = time()
            java_evaluator.evaluate(JArray((JArray)(JDouble))(layout))
            fits[p] = java_evaluator.getEnergyCost()
            all_fit.append(fits[p])
            # text_fits.write(str(fits[p]))

            # print('eval:',i,'p:',p,'time:',time()-t ,'fit:',fits[p],'minfit:',min(fits))
            # t = time()

            if fits[p] < best_fit:
                best_layout = layout

                best_fit = fits[p]
        
        with open('test_ga_layout_100000.csv','w+') as f:
            text_layout = csv.writer(f)
            text_layout.writerows(best_layout)

        with open('test_ga_fits_100000.csv','a+') as f:
            a = csv.writer(f)
            a.writerow(fits)
        

        minfit = min(fits)
        # all_fit.append(minfit)
        print('min fit: ','%d\t%f' % (i,minfit),time()-run_time)

    print('final minfit:',min(all_fit))
    return best_layout,all_fit
    # plotg(best_layout,all_fit)
    # print('final minfit:',min(all_fit))


# run_ga(wind_scenario,java_evaluator)



# if __name__=='__main__':
#     import os
#     import jpype  # use jpype library python calling java function

#     save_path = 'data/ga'
#     # set jdk location
#     os.environ['JAVA_HOME'] = '/Library/Java/JavaVirtualMachines/jdk-20.0.1.jdk/'

#     # call java jar document
#     # jarpath=r'/Users/lifangsai/Desktop/postgraduation/project/WindFLO-code2/Javanew/out/artifacts/Evaluate/Javanew.jar'
#     jarpath=r'../Java/Javanew.jar'


#     # run jar file use JVM
#     jpype.startJVM(jpype.getDefaultJVMPath(), "-ea", "-Djava.class.path=%s" %jarpath)

#     # get need class from java
#     evaluator =jpype.JClass('KusiakLayoutEvaluator')
#     java_evaluator = evaluator() # instance

#     for i in range(0,5):
#         path = '/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind Competition/2015/Scenarios/%s.xml'%i
#         ws = WindScenario(path)
#         java_evaluator.initialize(path) # if use parameters should use JArray((JArray)(JDouble))(layout) to transform type 
#         (best_layout,all_fit) = run_ga(ws,java_evaluator)
        
#         savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
#         savedata(save_path+"/all_fits%s.csv"%i,all_fit)


#         all_fit = []

        

