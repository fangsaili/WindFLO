# import algorithms
from GA import run_ga
from SA import *
from SA_perturbation import run_sa_perturbation
from newAL import *
from newSA import *
from TA_standard import *
from TA_standard1 import *
from TA_standard2 import *
from TA_standard_right import *
from TA_standard_right1 import *
from TA_standard_right2 import *
from TA_distance import run_ta_distance




# import environment
import numpy as np
from KusiakEvaluator import WindScenario

import os
import jpype  # use jpype library python calling java function

from plotGragh import plotg,savedata,savedatas


if __name__=='__main__':
    
    model = 'TA_distance'


    # set jdk location
    os.environ['JAVA_HOME'] = '/Library/Java/JavaVirtualMachines/jdk-20.0.1.jdk/'

    # call java jar document
    # jarpath=r'/Users/lifangsai/Desktop/postgraduation/project/WindFLO-code2/Javanew/out/artifacts/Evaluate/Javanew.jar'
    jarpath=r'../Java/Javanew.jar'

    # run jar file use JVM
    jpype.startJVM(jpype.getDefaultJVMPath(), "-ea", "-Djava.class.path=%s" %jarpath)

    # get need class from java
    evaluator =jpype.JClass('KusiakLayoutEvaluator')
    java_evaluator = evaluator() # instance


    for i in range(1,2):
        path = '/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind_Competition/2015/Scenarios/%s.xml'%i
        senario_path = '../Wind_Competition/2015/Scenarios/%s.xml'%i


        if model == 'SA':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_sa(grid,java_evaluator)
            # plotg(best_layout,all_fits)
            
            save_path = 'data/SA'
            savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits%s.csv"%i,all_fits)

        if model == 'SA_perturbation':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_sa_perturbation(grid,java_evaluator)
            # plotg(best_layout,all_fits)
            
            save_path = 'data/SA_perturbation'
            savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits%s.csv"%i,all_fits)


        if model == 'newSA':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_new_sa(grid,java_evaluator,n_final=100000)
            # plotg(best_layout,all_fits)
            
            save_path = 'data/newSA/SA1'
            savedatas(save_path+"/best3_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all3_fits%s.csv"%i,all_fits)


        if model == 'GA':
            ws = WindScenario(senario_path)
            java_evaluator.initialize(senario_path) # if use parameters should use JArray((JArray)(JDouble))(layout) to transform type 
            (best_layout,all_fits) = run_ga(ws,java_evaluator)
            save_path = 'data/ga/onehs'
            savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits%s.csv"%i,all_fits)

        if model == 'newAL':
            ws = WindScenario(senario_path)
            java_evaluator.initialize(senario_path)
            initial_grid = generate_initial_grid(ws)
            (best_layout,all_fits) = new_Al_angle(ws,initial_grid,java_evaluator)

            save_path = 'data/newAL/angle'
            savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits%s.csv"%i,all_fits)


        if model == 'TA_standard':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard'
            savedatas(save_path+"/best_layout1000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits1000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_standard1':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard1(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard1'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits10000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_standard2':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard2(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard2'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits10000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_standard_right':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard_right(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard_right'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits100000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_standard_right1':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard_right1(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard_right1'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits100000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_standard_right2':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_standard_right2(grid,java_evaluator,n_final=100000,cycle_limit=1000)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_standard_right2'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits100000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)

        if model == 'TA_distance':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            x = len(np.arange(0, ws.width, 8.001*ws.R))
            y = len(np.arange(0, ws.height, 8.001*ws.R))

            grid = generate_grid(ws)
            (best_layout,all_fits) = run_ta_distance(grid,java_evaluator,alpha=0.9,n_final=2000,cycle_limit=10,width=x,height=y)
            # plotg(best_layout,all_fits)

            save_path = 'data/TA_distance'
            savedatas(save_path+"/best_layout100000_%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits10000_%s.csv"%i,all_fits)
            plotg(best_layout,all_fits)







    # for i in range(0,5):
    #     # path = '/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind Competition/2015/Scenarios/%s.xml'%i
    #     senario_path = '../Wind Competition/2015/Scenarios/%s.xml'%i

    #     ws = WindScenario(senario_path)
    #     java_evaluator.initialize(senario_path) # if use parameters should use JArray((JArray)(JDouble))(layout) to transform type 
    #     (best_layout,all_fit) = run_ga(ws,java_evaluator)
        
    #     savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
    #     savedata(save_path+"/all_fits%s.csv"%i,all_fit)


    #     # all_fit = []