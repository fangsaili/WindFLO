import numpy as np
import math
import xml.etree.ElementTree as ET

class WindScenario:

    def __init__(self,xml):
        # use xml library load parameters from xml document
        root = ET.parse(xml).getroot()
        scenario = {'c':[],
                'k':[],
                'omega':[],
                'theta':[],
                'obstacle': []}
        for child in root:
            if child.tag == 'Angles':
                for angle in child:
                    for param in angle.items():
                        scenario[param[0]].append(eval(param[1]))
            if child.tag == 'Obstacles':
                for obstacle in child:
                    obsdict = {}
                    for param in obstacle.items():
                        obsdict[param[0]]=eval(param[1])
                    scenario['obstacle'].append(obsdict)
            if child.tag == 'Parameters':
                for param in child:
                    scenario[param.tag] = eval(param.text)
        
        # senario wind parameter           
        self.cs = np.array(scenario['c'])
        self.ks = np.array(scenario['k'])
        self.omegas = np.array(scenario['omega'])
        thetas = np.array(scenario['theta'])
        self.thetas = np.vstack((thetas, thetas+15)).T

        # senario farm parameter
        self.obstacles = scenario['obstacle']
        self.width = scenario['Width']
        self.height = scenario['Height']
        self.nturbines = scenario['NTurbines']
        self.wakeFreeEnergy = scenario['WakeFreeEnergy']

        # senario default parameters
        self.CT=0.8
        self.PRated=1500.0
        self.R=38.5
        self.eta=-500.0
        self.k=0.0750
        self.slambda=140.86
        self.vCin=3.5
        self.vCout=20
        self.vRated=14

        # optimization parametrs
        self._dcos = []
        self._esin = []
        self.rkRatio = self.R/self.k
        self.krRatio = self.k/self.R
        self.vints = []
        self.atan_k = math.atan(self.k)
        self.trans_CT = 0
        self.minDist = 0

        self.initOptimizationparameters()

    def initOptimizationparameters(self):
        # set dcos, esin, v1large, v2large, Plarge
        fac = np.pi/180
        self.nturb = self.nturbines
        self.ntheta = np.size(self.thetas)
        self._dcos = [ np.cos(t.mean()*fac)  for t in self.thetas]
        self._esin = [ np.sin(t.mean()*fac)  for t in self.thetas]
        self.vints = np.arange(3.5, self.vRated+0.5, 0.5)
        self.trans_CT = 1 - np.sqrt(1 - self.CT)
        self.minDist = 64*self.R*self.R

class WindFarmLayoutEvaluator:

    def __init__(self, ws):
        self.ws = ws
        self._evals = 0

    def settings(self, nturb):
        # set dcos, esin, v1large, v2large, Plarge
        fac = np.pi/180
        ntheta = np.size(self.ws.thetas)
        self._dcos = np.tile(np.reshape(np.tile(np.cos(np.mean(
            self.ws.thetas*fac,1)), (nturb, 1)).T,
            ntheta*nturb/2, 1), (nturb, 1))
        self._esin = np.tile(np.reshape(np.tile(np.sin(np.mean(
            self.ws.thetas*fac,1)), (nturb, 1)).T,
            ntheta*nturb/2, 1), (nturb, 1))
        vints = numpy.arange(3.5, self.ws.vRated+0.5, 0.5)
        self._v1large = np.tile(np.array(vints[1:-1]), (ntheta, nturb))
        self._v2large = np.tile(np.array(vints[0:-2]), (ntheta, nturb))

    def evaluate(self, layout):
        self._evals += 1
        if not self.check_constraint(layout):
            return float('infinity')
        return 0

    def check_constraint(self, layout):
        dists = scipy.spatial.distance.pdist(layout)
        if any(dists <= 8*self.ws.R):
            return False
        for obs in self.ws.obstacles:
            bpositions = np.multiply(
                    layout <= [obs['xmax'], obs['ymax']],
                    layout >= [obs['xmin'], obs['ymin']])
            if any(np.multiply(bpositions[:,0], bpositions[:,1])):
                return False
        return True
