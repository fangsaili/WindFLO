/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

************************************************************************************/


#include "utils.h"
#include "LocalSearch.h"
#include "FitnessCalculation.h"
#include <algorithm>
#include <limits.h>
#include <float.h>
using namespace std;

double applyLocalSearchRealEvaluator(vector < pair <double, double> > &turbines, int width, int height, double stepSize, int remEvaluations, double R, KusiakLayoutEvaluator &wfle, vector<Obstacle> &obstacles){
	//Get current efficiencies
	double energy, currentFitness;
	getDataInReal(turbines, wfle, energy, currentFitness);
	int evaluated = 1;
	vector < double > currentEfficiencies(turbines.size());
	for (int i = 0; i < turbines.size(); i++){
		currentEfficiencies[i] = wfle.getTurbineFitnesses()->get(i, 0);
	}
	
	//Several turbines are moved simultaneously
	double safeDistance = 5 * 8 * R;
	double discretizationDistance = 6 * 8 * R;
	while(evaluated + 2 <= remEvaluations){
		int selected = getRandomInteger0_N(turbines.size() - 1);
		//Select one turbine randomly, then discretize the space to select a set of centers of the regions
		//Calculate the centers
		double selectedFirst = turbines[selected].first;
		double selectedSecond = turbines[selected].second;
		vector < pair < double, double> > centers;
		int maxSteps = sqrt(height * height + width * width) / discretizationDistance + 1;
		for (double positionFirst = selectedFirst - maxSteps * discretizationDistance; selectedFirst <= selectedFirst + maxSteps * discretizationDistance; positionFirst += discretizationDistance){
				if (positionFirst < 0){
					continue;
				}
				if (positionFirst > width){
					break;
				}
			for (double positionSecond = selectedSecond - maxSteps * discretizationDistance; selectedSecond <= selectedSecond + maxSteps * discretizationDistance; positionSecond += discretizationDistance){
				if (positionSecond < 0){
					continue;
				}
				if (positionSecond > height){
					break;
				}
				centers.push_back(make_pair(positionFirst, positionSecond));
			}
		}
		//Find the turbine associated to each center
		vector<int> selectedTurbines;
		for (int i = 0; i < centers.size(); i++){
			double minDistance = DBL_MAX;
			int minIndex = -1;
			for (int j = 0; j < turbines.size(); j++){
				double distSquare = (centers[i].first - turbines[j].first) * (centers[i].first - turbines[j].first) + (centers[i].second - turbines[j].second) * (centers[i].second - turbines[j].second);
				if (distSquare < minDistance){
					minDistance = distSquare;
					minIndex = j;
				}
			}
			bool allOk = true;
			for (int j = 0; j < selectedTurbines.size(); j++){
				int selected = selectedTurbines[j];
				double firstPrev = turbines[selected].first;
				double secondPrev = turbines[selected].second;
				double firstCurr = turbines[minIndex].first;
				double secondCurr = turbines[minIndex].second;
				double dist = (firstPrev - firstCurr) * (firstPrev - firstCurr) + (secondPrev - secondCurr) * (secondPrev - secondCurr);
				if (dist * dist < safeDistance * safeDistance){
					allOk = false;
				}
			}
			if (allOk){
				selectedTurbines.push_back(minIndex);
			}
		}
		//Move each turbine
		vector < pair <double, double> > newTurbines = turbines;
		vector < bool > changed(selectedTurbines.size(), false);
		int changes = 0;
		for (int i = 0; i < selectedTurbines.size(); i++){
			int numTries = 10;
			int changedIndex = selectedTurbines[i];
			for (int j = 0; j < numTries; j++){
				double angle = generateRandomDouble0_Max(2*M_PI);
				double nextSize = generateRandomDouble0_Max(stepSize);
				double newFirst = newTurbines[changedIndex].first + nextSize * cos(angle);
				double newSecond = newTurbines[changedIndex].second + nextSize * sin(angle);
				bool allOk = true;
				if ((newFirst < 0) || (newFirst > width) || (newSecond < 0) || (newSecond > height)){
					allOk = false;
				}
				for (int i = 0; i < obstacles.size(); i++){
					if ((newFirst > obstacles[i].minFirst) && (newFirst < obstacles[i].maxFirst) && (newSecond > obstacles[i].minSecond) && (newSecond < obstacles[i].maxSecond)){
						allOk = false;	
					}
				}
				if (allOk){
					for (int k = 0; k < newTurbines.size(); k++){
						if (k == changedIndex){
							continue;
						}
						double distSq = (newTurbines[k].first - newFirst) * (newTurbines[k].first - newFirst) + (newTurbines[k].second - newSecond) * (newTurbines[k].second - newSecond);
						if (distSq - 1 < 8 * 8 * R * R ){
							allOk = false;
							break;
						}
					}
					if (allOk){
						newTurbines[changedIndex] = make_pair(newFirst, newSecond);
						changed[i] = true;
						changes++;
						break;
					}
				}
			}
		}
		double energyNew, fitnessNew;
		getDataInReal(newTurbines, wfle, energyNew, fitnessNew);
		evaluated++;
		if (fitnessNew < 1e-10){//No valido
			continue;
		}

		vector < double > newEfficiencies(turbines.size());
		for (int i = 0; i < turbines.size(); i++){
			newEfficiencies[i] = wfle.getTurbineFitnesses()->get(i, 0);
		}
		//Evaluate each change
		double estimatedChange = 0;
		int validated = 0;
		for (int i = 0; i < changed.size(); i++){
			if (!changed[i]){
				continue;
			}
			int changedIndex = selectedTurbines[i];
			double efficiencyOld = 0, efficiencyNew = 0;
			//Efficiency in the distributions
			for (int j = 0; j < turbines.size(); j++){
				double dist1Sq = (turbines[changedIndex].first - turbines[j].first) * (turbines[changedIndex].first - turbines[j].first) + (turbines[changedIndex].second - turbines[j].second) * (turbines[changedIndex].second - turbines[j].second);
				double dist2Sq = (newTurbines[changedIndex].first - turbines[j].first) * (newTurbines[changedIndex].first - turbines[j].first) + (newTurbines[changedIndex].second - turbines[j].second) * (newTurbines[changedIndex].second - turbines[j].second);
				double minDistSq = min(dist1Sq, dist2Sq);
				if (minDistSq < safeDistance * safeDistance / 4){
					efficiencyOld += currentEfficiencies[j];
					efficiencyNew += newEfficiencies[j];
				}
			}
			if (efficiencyOld > efficiencyNew){
				newTurbines[changedIndex] = turbines[changedIndex];//Revert change
			} else {
				validated++;
			}
		}
		//Reevaluate
		if (validated > 0){
			double energyValidated, fitnessValidated;
			getDataInReal(newTurbines, wfle, energyValidated, fitnessValidated);
			evaluated++;
			if (fitnessValidated < currentFitness){
				currentFitness = fitnessValidated;
				cout << "Found: " << currentFitness << endl;
				turbines = newTurbines;
				for (int i = 0; i < turbines.size(); i++){
					currentEfficiencies[i] = wfle.getTurbineFitnesses()->get(i, 0);
				}
			}
		}
	}
	return currentFitness;
}

double applySearchNearObstacles(vector < pair <double, double> > &turbines, int width, int height, int remEvaluations, int &evaluations, double R, KusiakLayoutEvaluator &wfle, vector < Obstacle > &obstacles){
	double currentEnergy, currentFitness;
	getDataInReal(turbines, wfle, currentEnergy, currentFitness);
	evaluations++;
	if (obstacles.size() == 0){
		return currentFitness;
	}
	remEvaluations--;
	int pointsPerSide = remEvaluations / (obstacles.size() * 4);
	vector < pair < double, double > > pointsToCheck;
	for (int i = 0; i < obstacles.size(); i++){
		double divSizeFirst = (obstacles[i].maxFirst - obstacles[i].minFirst) / (pointsPerSide + 1);
		for (int j = 1; j <= pointsPerSide; j++){
			pointsToCheck.push_back(make_pair(obstacles[i].minFirst + divSizeFirst * j, obstacles[i].minSecond));
			pointsToCheck.push_back(make_pair(obstacles[i].minFirst + divSizeFirst * j, obstacles[i].maxSecond));
		}
		double divSizeSecond = (obstacles[i].maxSecond - obstacles[i].minSecond) / (pointsPerSide + 1);
		for (int j = 1; j <= pointsPerSide; j++){
			pointsToCheck.push_back(make_pair(obstacles[i].minFirst, obstacles[i].minSecond + divSizeSecond * j));
			pointsToCheck.push_back(make_pair(obstacles[i].maxFirst, obstacles[i].minSecond + divSizeSecond * j));
		}
	}
	random_shuffle(pointsToCheck.begin(), pointsToCheck.end());
	for (int i = 0; i < pointsToCheck.size(); i++){
		for (int j = 0; j < 2; j++){
			int selectedToRemove = getRandomInteger0_N(turbines.size() - 1);
			pair<double, double> removed = turbines[selectedToRemove];
			turbines[selectedToRemove] = pointsToCheck[i];
			bool allOk = true;
			for (int k = 0; k < turbines.size(); k++){
				if (k == selectedToRemove)
					continue;
				double distSq = (turbines[k].first - turbines[selectedToRemove].first) * (turbines[k].first - turbines[selectedToRemove].first) + (turbines[k].second- turbines[selectedToRemove].second) * (turbines[k].second- turbines[selectedToRemove].second);
				if (distSq - 5 < 8 * 8 * R * R ){
					allOk = false;
					break;
				}
			}
			if (allOk){
				double newEnergy, newFitness;
				getDataInReal(turbines, wfle, newEnergy, newFitness);
				evaluations++;
				if (newFitness < 1e-10){//Invalid
					newFitness = currentFitness + 1;
				}
				if (newFitness < currentFitness){
					currentFitness = newFitness;
					cout << "Found: " << currentFitness << endl;
				} else {
					//Recover
					turbines[selectedToRemove] = removed;
				}
			} else {
				//Recover
				turbines[selectedToRemove] = removed;
			}
		}
	}
	return currentFitness;
}
