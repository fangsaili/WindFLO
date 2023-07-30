/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre
**************************************************************************************/

#include "SolutionSimpleModel.h"
#include "utils.h"

#include <limits.h>
#include <vector>
#include <math.h>
#include <iostream>

TurbineEstimator *SolutionSimpleModel::turbineEstimator = NULL;

void SolutionSimpleModel::initializeTurbineEstimator(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector <Obstacle> &obstacles){
	turbineEstimator = new TurbineEstimator(wsc, wfle, obstacles);
}

SolutionSimpleModel::SolutionSimpleModel(int w, int h, WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector < pair <double, double> > &turbines, vector < Obstacle > &obstacles){
	R = wsc.R;
	conflicts = 0;
	if (turbineEstimator == NULL){
		initializeTurbineEstimator(wsc, wfle, obstacles); 
	}
	initialize(w, h, turbineEstimator);
	for (int i = 0; i < turbines.size(); i++){
		insert(turbines[i].first, turbines[i].second);
	}
}

void SolutionSimpleModel::clear(){
	initialize(width, height, turbineEstimator);
}

void SolutionSimpleModel::initialize(int w, int h, TurbineEstimator *t){
	conflicts = 0;
	currentTurbinesToEnergy.clear();
	width = w;
	height = h;
	turbineEstimator = t;
	currentEnergy = 0;
	if (w % 35 == 0){
		regionSizeX = w / 35;
	} else {
		regionSizeX = w / 35 + 1;
	}
	if (h % 35 == 0){
		regionSizeY = h / 35;
	} else {
		regionSizeY = h / 35 + 1;
	}
}

void SolutionSimpleModel::insert(double  x, double  y){
	if (currentTurbinesToEnergy.count(make_pair(x,y))){
		cerr << "Internal error. Turbine (" << x << ", " << y << ") was already in the solution" << endl;
		exit(-1);
	}
	currentTurbinesToEnergy[make_pair(x,y)] = 1;
	map < pair <double, double>, double >::iterator itxy = currentTurbinesToEnergy.find(make_pair(x,y));

	int minX = max((int)(x - R * 8 * 5 - 1) / regionSizeX, 0);
	int maxX = min((int)(x + R * 8 * 5 + 1) / regionSizeX, NUM_REGIONS - 1);
	int minY = max((int)(y - R * 8 * 5 - 1) / regionSizeY, 0);
	int maxY = min((int)(y + R * 8 * 5 + 1) / regionSizeY, NUM_REGIONS - 1);
	for (int i = minX; i <= maxX; i++){
		for (int j = minY; j <= maxY; j++){
			for (set < pair<double, double> >::iterator it = regionsToTurbines[i][j].begin(); it != regionsToTurbines[i][j].end(); it++){
				double oldX = it->first;
				double oldY = it->second;
				if ((oldX != x) || (oldY != y)){
					double distance = (oldX - x) * (oldX - x) + (oldY - y) * (oldY - y);
					double angle1 = atan2((oldY - y), (oldX - x));
					if (angle1 < 0){
						angle1 += 2 * M_PI;
					}
					angle1 = (angle1) * 180.0 / M_PI;
					double angle2 = angle1 + 180;
					if (angle2 >= 360){
						angle2 -= 360;
					}
					double sqrtdistance = sqrt(distance);
					double predic1 = turbineEstimator->estimate(sqrtdistance, angle1);
					double predic2 = turbineEstimator->estimate(sqrtdistance, angle2);
					if (predic1 < 0){
						conflicts++;
					}
					map < pair <double, double>, double >::iterator itoldxoldy = currentTurbinesToEnergy.find(make_pair(oldX,oldY));
					double orig = itoldxoldy->second;
					//double orig = currentTurbinesToEnergy[make_pair(oldX,oldY)];
					itoldxoldy->second -= (1 - predic1);
					//currentTurbinesToEnergy[make_pair(oldX,oldY)] -= (1 - predic1);
					currentEnergy -= (orig - itoldxoldy->second);
					//currentEnergy -= (orig - currentTurbinesToEnergy[make_pair(oldX,oldY)]);
					itxy->second -= (1 - predic2);
					//currentTurbinesToEnergy[make_pair(x, y)] -= (1 - predic2);
				}	
			}
		}
	}
	currentEnergy += currentTurbinesToEnergy[make_pair(x,y)];
	regionsToTurbines[(int)x / regionSizeX][(int)y / regionSizeY].insert(make_pair(x, y));
}

void SolutionSimpleModel::remove(double  x, double  y){
	map < pair <double, double>, double >::iterator itxy = currentTurbinesToEnergy.find(make_pair(x,y));
	//if (currentTurbinesToEnergy.count(make_pair(x,y)) == 0){
	if (itxy == currentTurbinesToEnergy.end()){
		cerr << "Internal error, turbine " << x << ", " << y << " does not exist" << endl;
		exit(-1);
	}
	currentEnergy -= itxy->second;
	//currentEnergy -= currentTurbinesToEnergy[make_pair(x,y)];
	currentTurbinesToEnergy.erase(itxy);
	//currentTurbinesToEnergy.erase(make_pair(x,y));

	regionsToTurbines[(int)x / regionSizeX][(int)y / regionSizeY].erase(make_pair(x, y));
	int minX = max((int)(x - R * 8 * 5 - 1) / regionSizeX, 0);
	int maxX = min((int)(x + R * 8 * 5 + 1) / regionSizeX, NUM_REGIONS - 1);
	int minY = max((int)(y - R * 8 * 5 - 1) / regionSizeY, 0);
	int maxY = min((int)(y + R * 8 * 5 + 1) / regionSizeY, NUM_REGIONS - 1);
	int count = 0;
	for (int i = minX; i <= maxX; i++){
		for (int j = minY; j <= maxY; j++){
			for (set < pair<double, double> >::iterator it = regionsToTurbines[i][j].begin(); it != regionsToTurbines[i][j].end(); it++){
				double oldX = it->first;
				double oldY = it->second;
				double distance = (oldX - x) * (oldX - x) + (oldY - y) * (oldY - y);
				double angle1 = atan2((oldY - y), (oldX - x));
				if (angle1 < 0){
					angle1 += 2 * M_PI;
				}
				angle1 = (angle1) * 180.0 / M_PI;
				double predic1 = turbineEstimator->estimate(sqrt(distance), angle1);
				if (predic1 < 0){
					conflicts--;
				}
				map < pair <double, double>, double >::iterator itoldxoldy = currentTurbinesToEnergy.find(make_pair(oldX,oldY));
				double orig = itoldxoldy->second;
				//double orig = currentTurbinesToEnergy[make_pair(oldX,oldY)];
				itoldxoldy->second += (1 - predic1);
				//currentTurbinesToEnergy[make_pair(oldX,oldY)] += (1 - predic1);
				currentEnergy += (itoldxoldy->second - orig);
				//currentEnergy += (currentTurbinesToEnergy[make_pair(oldX,oldY)] - orig);
			}
		}
	}
}

double SolutionSimpleModel::checkExchange(double oldX, double  oldY, double  newX, double  newY){
	if (currentTurbinesToEnergy.count(make_pair(newX, newY))){//newX, newY already exists, just return a negative number
		cerr << "Internal error: it is trying to insert a turbine that already exist" << endl;
		exit(-1);
	}
	if (currentTurbinesToEnergy.count(make_pair(oldX, oldY)) == 0){
		cerr << "Internal error: it is trying to remove a turbine that does not exist" << endl;
		exit(-1);
	}

	double copyCurrent = currentEnergy;
	remove(oldX, oldY);
	insert(newX, newY);
	double newEnergy = currentEnergy;
	remove(newX, newY);
	insert(oldX, oldY);
	return (newEnergy - copyCurrent);
}


void SolutionSimpleModel::getCurrentTurbines(vector < pair < double, double> > &turbines){
	turbines.clear();
	for (map< pair<double, double>, double>::iterator it = currentTurbinesToEnergy.begin(); it != currentTurbinesToEnergy.end(); it++){
		turbines.push_back(it->first);
	}
}
