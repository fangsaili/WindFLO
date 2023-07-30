/****************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description: estimator of the efficiency of a turbine when placed in a given position
 with respect to other turbine
 It uses linear interpolation
 *****************************************************************************************/

#ifndef __TURBINE_ESTIMATOR_H__
#define __TURBINE_ESTIMATOR_H__

#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include "Obstacle.h"

class TurbineEstimator {	
	public:
		TurbineEstimator(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector<Obstacle> &obstacles);
		double estimate(double distance, double angle);
		double getR() { return R; }
		void calculateFitness(double dx, double dy, double &f1, double &f2, vector < Obstacle > &obstacles, WindScenario &wsc, KusiakLayoutEvaluator &wfle);
		double getWakeFreeEnergy() { return wakeFreeEnergy; }
		int getPerformedEvaluations() { return evaluations; }

	private:
		double fitnessR[720], fitness1_5R[360], fitness2R[360], fitness2_5R[360], fitness3R[360], fitness3_5R[360], R;
		double wakeFreeEnergy;
		bool wakeFreeCalculated;
		int evaluations;
};

#endif
