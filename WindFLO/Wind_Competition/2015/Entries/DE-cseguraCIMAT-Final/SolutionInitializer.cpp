/****************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre
*****************************************************************************************/

#include <iostream>
#include <map>
#include <strings.h>
#include <limits.h>
#include <float.h>
#include <algorithm>
#include "SolutionInitializer.h"
#include "utils.h"
#include "FitnessCalculation.h"

//Create solution based on the romboid representation and do an "adaptive local Search" using the estimator
//In the first phases, the moves are large and at the end the moves are small
void createSolutionRomboidImproved(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector < pair<double, double> > &bestTurbines, double &deg1, double &deg2, double &dist1, double &dist2, vector<Obstacle> &obstacles, int lastEval, bool randomInit){
	double best = DBL_MAX;
	double bestReal = DBL_MAX;


	if (randomInit){
		dist1 = wsc.R * 8 + 1 + generateRandomDouble0_Max(wsc.R * 8 * 2);
		dist2 = wsc.R * 8 + 1 + generateRandomDouble0_Max(wsc.R * 8 * 2);
		deg1 = generateRandomDouble0_Max(90);
		deg2 = 70 + generateRandomDouble0_Max(40);
	}
	double newdist1 = dist1;
	double newdist2 = dist2;
	double newdeg1 = deg1;
	double newdeg2 = deg2;

	for (int i = 0; i < lastEval; i++){
		int converter = i;
		vector < pair < double, double > > turbines;
		if (createSolutionRomboid(newdeg1, newdeg2, newdist1, newdist2, wsc, turbines, obstacles)){
			double energy, fitness;
			double energyReal, fitnessReal;
			if (turbines.size() > 10000){
				turbines.resize(1000);
			}
			SolutionSimpleModel s (wsc.width, wsc.height, wsc, wfle, turbines, obstacles);
			if (s.isAdmissible()){
				getDataInSimplified(s, wsc, energy, fitness, SolutionSimpleModel::turbineEstimator->getWakeFreeEnergy());
				fitnessReal = fitness;
			} else {
				fitness = DBL_MAX;
				fitnessReal = DBL_MAX;
			}

			if (fitnessReal < bestReal){
				best = fitness;
				bestReal = fitnessReal;
				dist1 = newdist1;
				dist2 = newdist2;
				deg1 = newdeg1;
				deg2 = newdeg2;
				bestTurbines = turbines;
			}
		}
		double p1, p2, p3, p4;
		do {
			p1 = generateRandomDouble0_Max(1);
			p2 = generateRandomDouble0_Max(1);
			p3 = generateRandomDouble0_Max(1);
			p4 = generateRandomDouble0_Max(1);
		} while((p1 >= 0.25) && (p2 >= 0.25) && (p3 >= 0.25) && (p4 >= 0.25));
		newdeg1 = deg1;
		newdeg2 = deg2;
		newdist1 = dist1;
		newdist2 = dist2;
		if (p1 <= 0.25){
			generateRandomDouble0_Max(1);
			double maxChange = 45 - 45.0 * converter / (lastEval + 1);
			newdeg1 = deg1 + generateRandomDouble0_Max(maxChange) - maxChange / 2.0;
			newdeg1 = max(newdeg1, 0.0);
			newdeg1 = min(newdeg1, 90.0);
			newdeg2 -= (newdeg1 - deg1);
			newdeg2 = max(newdeg2, 30.0);
			newdeg2 = min(newdeg2, 150.0);
		} 
		if (p2 <= 0.25){
			double maxChange = 45 - 45.0 * converter / (lastEval + 1);
			newdeg2 = deg2 + generateRandomDouble0_Max(maxChange) - maxChange / 2.0;
			newdeg2 = max(newdeg2, 30.0);
			newdeg2 = min(newdeg2, 150.0);
		} 
		if (p3 <= 0.25){
			double maxChange = 250 - 250.0 * converter / (lastEval + 1); 
			newdist1 = max(dist1 + generateRandomDouble0_Max(maxChange) - maxChange / 2.0, wsc.R * 8 + 0.1);
			newdist1 = max(newdist1, 8 * wsc.R + 1);
			newdist1 = min(newdist1, 5 * 8 * wsc.R + 1);
		}
		if (p4 <= 0.25){
			double maxChange = 250 - 250.0 * converter / (lastEval + 1); 
			newdist2 = max(dist2 + generateRandomDouble0_Max(maxChange) - maxChange / 2.0, wsc.R * 8 + 0.1);
			newdist2 = max(newdist2, 8 * wsc.R + 1);
			newdist2 = min(newdist2, 5 * 8 * wsc.R + 1);
		}
	}
	cout << "Falso: " << best << endl;
}

bool createSolutionRomboid(double deg1, double deg2, double dist1, double dist2, WindScenario &wsc, vector < pair<double, double> > &turbines, vector<Obstacle> &obstacles){
	turbines.clear();
	double rad1 = deg1 * M_PI / 180;
	double rad2 = deg2 * M_PI / 180;
	double initPoint1 = 0;
	double initPoint2 = 0;
	int dispi = 0;
	double cosrad1 = cos(rad1);
	double sinrad1 = sin(rad1);
	double cosrad1rad2 = cos(rad1 + rad2);
	double sinrad1rad2 = sin(rad1 + rad2);
	for (int disp = -1; disp <= 1; disp += 2){
		bool enter = true;
		dispi = (disp == -1)?0:1;
		while(enter){	
			enter = false;
			double p1 = initPoint1 + dispi * dist1 * cosrad1;
			double p2 = initPoint2 + dispi * dist1 * sinrad1;
			int jForp1_0 = INT_MIN;
			int jForp1_width = INT_MAX;
			if (abs(cosrad1rad2) > 1e-4){
				jForp1_0 = (-1 - p1) / (dist2 * cosrad1rad2);
				jForp1_width = (wsc.width + 1 - p1) / (dist2 * cosrad1rad2);
			}
			int jForp2_0 = INT_MIN;
			int jForp2_height = INT_MAX;
			if (abs(sinrad1rad2) > 1e-4){
				jForp2_0 = (-1 - p2) / (dist2 * sinrad1rad2);
				jForp2_height = (wsc.height + 1 - p2) / (dist2 * sinrad1rad2);
			}
			int minj = max(min(jForp1_0, jForp1_width), min(jForp2_0, jForp2_height));
			int maxj = min(max(jForp1_0, jForp1_width), max(jForp2_0, jForp2_height));
			for (int j = minj - 1; j <= maxj + 1; j++){
				double newp1 = p1 + j * dist2 * cosrad1rad2;
				double newp2 = p2 + j * dist2 * sinrad1rad2;
				bool conflict = false;
				for (int i = 0; i < obstacles.size(); i++){
					if ((newp1 > obstacles[i].minFirst) && (newp1 < obstacles[i].maxFirst) && (newp2 > obstacles[i].minSecond) && (newp2 < obstacles[i].maxSecond)){
						conflict = true;
						break;
					}
				}

				if ((newp1 >= 0) && (newp2 >= 0) && (newp1 <= wsc.width) && (newp2 <= wsc.height) && (!conflict)){
					enter = true;
					turbines.push_back(make_pair(newp1, newp2));
					if (turbines.size() > 4000){
						enter = false;
						break;
					}
				}
			}
			dispi += disp;
		}
	}
	if (turbines.size() > 4000){//Bad configuration
		turbines.clear();
		return false;
	}
	random_shuffle(turbines.begin(), turbines.end());
	for (int i = 1; i < turbines.size(); i++){
		for (int j = 0; j < i; j++){
			double dist = (turbines[i].first - turbines[j].first) * (turbines[i].first - turbines[j].first) + (turbines[i].second - turbines[j].second) * (turbines[i].second - turbines[j].second);
			if (dist <= 8 * 8 * wsc.R * wsc.R + 0.25){//Safety threshold, required with the server
				//Remove turbine[i]
				turbines[i] = turbines.back();
				turbines.pop_back();
				i--;
				break;
			}
		}
	}
	return true;
}
