/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre
*************************************************************************************/

#include <limits.h>
#include <float.h>
#include "FitnessCalculation.h"

//Set of variables for the calculation of the fitness function
double ct  = 750000;
double cs  = 8000000;
double m   = 30;
double r   = 0.03;
double y   = 20;
double com = 20000;

//Obtain the fitness from the energy, number of turbines and wakeFreeEnergy of the scenario
double getFitness(int n, double energy, WindScenario &scenario, double wakeFreeEnergy){
	return (((ct*n+cs*std::floor(n/m))*(0.666667+0.333333*std::exp(-0.00174*n*n))+com*n)/
	  ((1.0-std::pow(1.0+r, -y))/r)/(8760.0*wakeFreeEnergy*energy))+0.1/n;
}

void getDataInReal(vector< pair<double, double> > &turbines, KusiakLayoutEvaluator &wfle, double &energy, double &fitness){
	if (turbines.size() == 0){
		energy = 0;
		fitness = DBL_MAX;
		return;
	}
	Matrix<double> solution(turbines.size(), 2);
	for (unsigned i = 0; i < turbines.size(); i++){
		solution.set(i, 0, turbines[i].first);
		solution.set(i, 1, turbines[i].second);
	}
	wfle.evaluate(&solution);
	energy = 0;
	Matrix<double>* turbinesFitnesses = wfle.getTurbineFitnesses();
	for (unsigned i = 0; i < turbines.size(); i++){
		energy += turbinesFitnesses->get(i, 0);
	}
	delete turbinesFitnesses;
	fitness = wfle.getEnergyCost();
}

void getDataInSimplified(SolutionSimpleModel &b, WindScenario &scenario, double &energy, double &fitness, double wakeFreeEnergy){
	energy = b.getCurrentEnergy();
	fitness = getFitness(b.getNumberOfTurbines(), energy, scenario, wakeFreeEnergy);
}
