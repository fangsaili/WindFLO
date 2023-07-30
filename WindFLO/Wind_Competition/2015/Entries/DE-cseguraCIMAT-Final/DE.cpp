/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
  It implements the Differential Evolution and at the end it tries to improve it
	by introducing turbines near the boundaries of obstacles and with a local search
	The DE is a memetic DE/Rand/1/bin
*************************************************************************************/

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <float.h>

#include "FitnessCalculation.h"
#include "SolutionInitializer.h"
#include "SolutionSimpleModel.h"
#include "TurbineEstimator.h"
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include "Matrix.hpp"
#include "LocalSearch.h"
#include "utils.h"
#include "Obstacle.h"
#include "Individual.h"


int main(int argc, const char * argv[]) {
	cout << fixed << setprecision(12);
	int popSize, maxEvaluations;
	double CR;
	if (argc == 4){
		srandom(atoi(argv[2]));
		maxEvaluations = atoi(argv[3]);
		popSize = 10;
		CR = 0.9;
	} else if (argc == 6){
		srandom(atoi(argv[2]));
		maxEvaluations = atoi(argv[3]);
		popSize = atoi(argv[4]);
		CR = atof(argv[5]);
	} else {
		cout << "Usage: " << argv[0] << " scenario seed maxEvaluations [popSize CR]" << endl;
		return 0;
	}

  WindScenario wsc(argv[1]);
  KusiakLayoutEvaluator wfle;
  wfle.initialize(wsc);
	//wfle.setRuntoken("AC5N56ZVCR34QDMQNMEP5RMZMNDU5X");

	//Fill obstacles
	vector<Obstacle> obstacles;
	for (unsigned int i = 0; i < wsc.obstacles.rows; i++){
		Obstacle obs;
		obs.minFirst = wsc.obstacles.get(i, 0);
		obs.minSecond = wsc.obstacles.get(i, 1);
		obs.maxFirst = wsc.obstacles.get(i, 2);
		obs.maxSecond = wsc.obstacles.get(i, 3);
		obstacles.push_back(obs);
	}
	
	//Initialize local model
	vector < pair < double, double > > turbinesEmpty;
	SolutionSimpleModel empty(wsc.width, wsc.height, wsc, wfle, turbinesEmpty, obstacles);
	int evaluations = SolutionSimpleModel::turbineEstimator->getPerformedEvaluations();

 	double bestReal = 1000000;
	double bestApprox = 1000000;
	vector < pair<double, double> > bestTurbines;

	double rangesMin[4] = {0, 30, 8 * wsc.R + 1, 8 * wsc.R + 1};
	double rangesMax[4] = {90, 150, 5 * 8 * wsc.R + 1, 5 * 8 * wsc.R + 1};

	vector<Individual> population;
	int creationSinceImprovement = 0;
	while (population.size() < 800){
		population.push_back(Individual());
		createSolutionRomboidImproved(wsc, wfle, population.back().turbines, population.back().parameters[0], population.back().parameters[1], population.back().parameters[2], population.back().parameters[3], obstacles, 15000, true);
		createSolutionRomboid(population.back().parameters[0], population.back().parameters[1], population.back().parameters[2], population.back().parameters[3], wsc, population.back().turbines, obstacles);
		SolutionSimpleModel solutionSimple(wsc.width, wsc.height, wsc, wfle, population.back().turbines, obstacles);
		double energy;
		getDataInSimplified(solutionSimple, wsc, energy, population.back().fitnessApprox, SolutionSimpleModel::turbineEstimator->getWakeFreeEnergy());
		evaluations++;
		getDataInReal(population.back().turbines, wfle, energy, population.back().fitnessReal);
		if (population.back().fitnessReal < 1e-10){//No valido
			population.back().fitnessReal = 1000000;
		}
		if (population.back().fitnessReal < bestReal){
			cout << "Found: " << population.back().fitnessReal << endl;
			creationSinceImprovement = 0;
			bestTurbines = population.back().turbines;
			bestReal = population.back().fitnessReal;
			bestApprox = population.back().fitnessApprox;
		} else {
			creationSinceImprovement++;
		}
	}
	sort(population.begin(), population.end(), cmpByReal);
	population.resize(popSize);

	int gen = 1;
	while((evaluations < maxEvaluations - 0.10 * maxEvaluations) && (gen < 2000)){
		cout << "Generation " << gen++ << endl;
		vector < Individual > offSpring(population.size());
		for (unsigned int candidate = 0; candidate < population.size(); candidate++){
  		//Select R values
		  unsigned int r1, r2, r3; 
			do{ 
				r1 = getRandomInteger0_N(popSize - 1); 
			} while(r1==candidate);
  		do {
		    r2 = getRandomInteger0_N(popSize - 1); 
			} while(r2== r1 || r2==candidate);
			do{ 
				r3 = getRandomInteger0_N(popSize - 1); 
			} while(r3==r2 || r3==r1 || r3==candidate);
			//Create trial
			offSpring[candidate] = population[candidate];
			int n = getRandomInteger0_N(3); 
      //double F = getRandomCauchy(0, 1);
			double F = generateRandomNormal(0.5, 0.3);
			for(int j = 0; j < 4; j++){
				if ((generateRandomDouble0_Max(1) < CR) || (j == n)){
					offSpring[candidate].parameters[j] = population[r1].parameters[j] + F * (population[r2].parameters[j] - population[r3].parameters[j]);
					if ((offSpring[candidate].parameters[j] < rangesMin[j]) || (offSpring[candidate].parameters[j] > rangesMax[j])){
						offSpring[candidate].parameters[j] = rangesMin[j] + generateRandomDouble0_Max(rangesMax[j] - rangesMin[j]);
					}
				}
			}
		}
		for (int i = 0; i < popSize; i++){
			//LS
			createSolutionRomboidImproved(wsc, wfle, offSpring[i].turbines, offSpring[i].parameters[0], offSpring[i].parameters[1], offSpring[i].parameters[2], offSpring[i].parameters[3], obstacles, 5000, false);
			SolutionSimpleModel solutionSimple(wsc.width, wsc.height, wsc, wfle, offSpring[i].turbines, obstacles);
			double energy;
			getDataInSimplified(solutionSimple, wsc, energy, offSpring[i].fitnessApprox, SolutionSimpleModel::turbineEstimator->getWakeFreeEnergy());
			if (offSpring[i].fitnessApprox < bestApprox * 1.20){
				double energy;
				evaluations++;
				getDataInReal(offSpring[i].turbines, wfle, energy, offSpring[i].fitnessReal);
				if (offSpring[i].fitnessReal < 1e-10){//No valido
					offSpring[i].fitnessReal = 1000000;
				}
				if (offSpring[i].fitnessReal < bestReal){
					cout << "Found: " << offSpring[i].fitnessReal << endl;
					bestTurbines = offSpring[i].turbines;
					bestReal = offSpring[i].fitnessReal;
					bestApprox = offSpring[i].fitnessApprox;
				} 
			} else {
				offSpring[i].fitnessReal = offSpring[i].fitnessApprox;
			}
			if (offSpring[i].fitnessReal <= population[i].fitnessReal){
				population[i] = offSpring[i];
			}
		}
	}

	//Local search arround the best found solution
	bestReal = applySearchNearObstacles(bestTurbines, wsc.width, wsc.height, (maxEvaluations - evaluations) / 2, evaluations, wsc.R, wfle, obstacles);
	bestReal = applyLocalSearchRealEvaluator(bestTurbines, wsc.width, wsc.height, 25 / 4.0, maxEvaluations - evaluations, wsc.R, wfle, obstacles);
	cout << "Solution: " << bestReal << endl;
	for (unsigned int i = 0; i < bestTurbines.size(); i++){
		cout << bestTurbines[i].first << " " << bestTurbines[i].second << endl;
	}
}
