# import algorithms
from GA import run_ga
from SA import *
from newAL import *
from newSA import *
# import environment
import numpy as np
from KusiakEvaluator import WindScenario

import os
import jpype  # use jpype library python calling java function

from plotGragh import plotg,savedata,savedatas


if __name__=='__main__':
    
    model = 'newSA'


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


    for i in range(4,5):
        path = '/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind Competition/2015/Scenarios/%s.xml'%i
        senario_path = '../Wind Competition/2015/Scenarios/%s.xml'%i


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

        if model == 'newSA':
            print(i)
            java_evaluator.initialize(senario_path)
            ws = WindScenario(senario_path)
            grid = generate_grid(ws)
            (best_layout,all_fits) = run_new_sa(grid,java_evaluator)
            # plotg(best_layout,all_fits)
            
            save_path = 'data/newSA'
            savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
            savedata(save_path+"/all_fits%s.csv"%i,all_fits)


        if model == 'GA':
            ws = WindScenario(senario_path)
            java_evaluator.initialize(senario_path) # if use parameters should use JArray((JArray)(JDouble))(layout) to transform type 
            (best_layout,all_fits) = run_ga(ws,java_evaluator)
            save_path = 'test/GA'
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








    # for i in range(0,5):
    #     # path = '/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind Competition/2015/Scenarios/%s.xml'%i
    #     senario_path = '../Wind Competition/2015/Scenarios/%s.xml'%i

    #     ws = WindScenario(senario_path)
    #     java_evaluator.initialize(senario_path) # if use parameters should use JArray((JArray)(JDouble))(layout) to transform type 
    #     (best_layout,all_fit) = run_ga(ws,java_evaluator)
        
    #     savedatas(save_path+"/best_layout%s.csv"%i,best_layout)
    #     savedata(save_path+"/all_fits%s.csv"%i,all_fit)


    #     # all_fit = []