/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
 
 Set of functions to get the energy and fitness of a given solution
 with the surrogated model
 The first time that the constructor is called, it initializes the estimator
 (turbineEstimator)
**************************************************************************************/

#ifndef __SOLUTION_SIMPLE_MODEL_H_
#define __SOLUTION_SIMPLE_MODEL_H_

#include "TurbineEstimator.h"
#include "Obstacle.h"
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include <float.h>
#include <vector>
#include <set>
#include <map>

#define NUM_REGIONS 100

using namespace std;

class SolutionSimpleModel{
	public:
		SolutionSimpleModel(){ fitness = DBL_MAX;}
		SolutionSimpleModel(int width, int height, WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector < pair <double, double> > &turbines, vector<Obstacle> &obstacles);
		void insert(double x, double y);
		void remove(double x, double y);
		double getCurrentEnergy() { return currentEnergy; }
		double getTurbineEnergy(pair <double, double> &turbine) { return currentTurbinesToEnergy[make_pair(turbine.first,turbine.second)]; }
		void clear();
		void initializeTurbineEstimator(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector<Obstacle> &obstacles);
		int getNumberOfTurbines() { return currentTurbinesToEnergy.size(); }
		void getCurrentTurbines(vector < pair < double, double> > &turbines);
		double checkExchange(double oldX, double oldY, double newX, double newY);
		int getWidth() { return width; }
		int getHeight() { return height; }
		void setFitness(double f) { fitness = f;}
		double getFitness() { return fitness; }
		bool isAdmissible() { return (conflicts == 0); }
		static TurbineEstimator *turbineEstimator;

	private:
		void initialize(int width, int height, TurbineEstimator *t);
		map < pair<double, double>, double> currentTurbinesToEnergy;
		int width, height;
		double currentEnergy;
		double fitness;
		double R;
		int conflicts;
		int regionSizeX, regionSizeY;
		set < pair<double, double> > regionsToTurbines[NUM_REGIONS+10][NUM_REGIONS+10];
};


#endif
