from jpype import JArray, JDouble
import numpy as np
from time import time

from KusiakEvaluator import WindScenario

import matplotlib.pyplot as plt


# set randon seed
np.random.seed(10086)

# generate initial layout grid 
def generate_initial_grid(wind_scenario):
    maxlength = (wind_scenario.width**2 + wind_scenario.height**2)**0.5/2
    xs = np.arange(wind_scenario.width*0.5-maxlength, wind_scenario.width*0.5+maxlength, 8.001*wind_scenario.R)
    ys = np.arange(wind_scenario.height*0.5-maxlength, wind_scenario.height*0.5+maxlength, 8.001*wind_scenario.R)
    x = np.tile(xs,(np.size(ys),1)).reshape(np.size(xs)*np.size(ys))
    y = np.tile(ys,(np.size(xs),1)).T.reshape(np.size(xs)*np.size(ys))
    grid = np.array([x,y])
    grid = grid.T
    return grid

# due to vector parameter to change initial layout
def new_grid(wind_scenario,grid,xx,xy,yx,yy):
    # return grid.dot(np.array([[1,0],[-0,1]]))
    # t = time()
    grid = grid.dot(np.array([[xx,xy],[yx,yy]]))
    bpositions = np.multiply(
            grid <= [wind_scenario.width,wind_scenario.height],
            grid >= [0,0])
    bina = np.multiply(bpositions[:,0], bpositions[:,1])
    grid = grid[bina,:]
    for obs in wind_scenario.obstacles:
        bpositions = np.multiply(
                grid <= [obs['xmax'], obs['ymax']],
                grid >= [obs['xmin'], obs['ymin']])
        bina = ~np.multiply(bpositions[:,0], bpositions[:,1])
        grid = grid[bina,:]

    # print(time()-t)
    return grid

# due to vector parameter to change initial layout
def new_grid_angle(wind_scenario,grid,xa,xs,ya,ys):
    # return grid.dot(np.array([[1,0],[-0,1]]))
    # t = time()
    
    
    xx = np.cos(xa/180*np.pi)*xs 
    xy = np.sin(xa/180*np.pi)*xs 
    yy = np.cos(ya/180*np.pi)*ys 
    yx = np.sin(ya/180*np.pi)*ys 
    # print(xx,xy,yx,yy)
    
    length = (abs(abs(xx)-abs(yx))**2 + abs(abs(xy)-abs(yy))**2)**0.5
    if length < 1 or xs < 1 or ys < 1:
        print('invalid',length)
        return None

    grid = grid.dot(np.array([[xx,xy],[yx,yy]]))
    bpositions = np.multiply(
            grid <= [wind_scenario.width,wind_scenario.height],
            grid >= [0,0])
    bina = np.multiply(bpositions[:,0], bpositions[:,1])
    grid = grid[bina,:]
    for obs in wind_scenario.obstacles:
        bpositions = np.multiply(
                grid <= [obs['xmax'], obs['ymax']],
                grid >= [obs['xmin'], obs['ymin']])
        bina = ~np.multiply(bpositions[:,0], bpositions[:,1])
        grid = grid[bina,:]

    # print(time()-t)
    return grid



def new_Al(scenrio,initial_grid,java_evaluator,xx=1,xy=0,yx=0,yy=1):

    vector = [xx,xy,yx,yy]
    delta_vector = [0.02,0.02,0.02,0.02]

    all_fits = []
    best_fit = 1
    best_layout = initial_grid

    all_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(initial_grid)))
    
    n = 0
    while n < 2000:
        for index,v in enumerate(delta_vector):

            
            vector[index] = vector[index] + v
            grid = new_grid(scenrio,initial_grid,vector[0],vector[1],vector[2],vector[3])
            all_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(grid)))

            while all_fits[-1] < all_fits[-2]:
                if all_fits[-1] < best_fit:
                    best_fit = all_fits[-1]
                    best_layout = grid
                vector[index] = vector[index] + v
                grid = new_grid(scenrio,initial_grid,vector[0],vector[1],vector[2],vector[3])
                all_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(grid)))
                n+=1
            else:
                vector[index] = vector[index] - v
                delta_vector[index] = -v
                n+=1
            # if all_fits[-1] > all_fits[-2]:
            #     vector[index] = vector[index] - v
            #     delta_vector[index] = -v
            # if all_fits[-1] < best_fit:
            #     best_fit = all_fits[-1]
            #     best_layout = grid
            print(n,all_fits[-1],best_fit,len(grid),vector,delta_vector)


    print()

    return best_layout,all_fits

#  10 4
#  15 3.9649
def new_Al_angle(scenrio,initial_grid,java_evaluator,xa=15,xs=1,ya=15,ys=1):

    vector = [xa,xs,ya,ys]
    delta_vector = [-1,0.1,-1,0.1]

    all_fits = []
    best_fit = 1
    best_layout = initial_grid

    # all_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(initial_grid)))
    n = 0
    for i in range(31):
        a = vector[0] - i
        for j in range(31):
            b = vector[2] - j

            grid = new_grid_angle(scenrio,initial_grid,a,vector[1],b,vector[3])

            all_fits.append(java_evaluator.evaluate(JArray((JArray)(JDouble))(grid)))
            if best_fit > all_fits[-1]:
                best_fit = all_fits[-1]
                best_layout = grid
            n += 1
            print(n,all_fits[-1],best_fit,len(grid),vector,delta_vector,i,j)




    return best_layout,all_fits



# senario_path = '../Wind Competition/2015/Scenarios/1.xml'
# ws = WindScenario(senario_path)
# grid = generate_initial_grid(ws)

# # plt.plot(grid[:,0],grid[:,1],'.')
# new = new_grid_angle(ws,grid,15,1,15,1)
# # # # print(new)
# plt.plot(new[:,0],new[:,1],'.')

# plt.show()