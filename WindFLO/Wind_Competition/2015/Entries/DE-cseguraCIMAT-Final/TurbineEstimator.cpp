/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre
*************************************************************************************/


#include "TurbineEstimator.h"
#include "utils.h"
#include <math.h>
#include <iostream>

using namespace std;

#define EPSILON 1

void TurbineEstimator::calculateFitness(double dx, double dy, double &f1, double &f2, vector < Obstacle > &obstacles, WindScenario &wsc, KusiakLayoutEvaluator &wfle){
	while(true){
		double p1first = generateRandomDouble0_Max(wsc.width);
		double p1second = generateRandomDouble0_Max(wsc.height);
		double p2first = p1first + dx;
		double p2second = p1second + dy;
		double ok = true;
		if ((p2first < 0) || (p2first > wsc.width) || (p2second < 0) || (p2second > wsc.height)){
			ok = false;
		}
		for (int i = 0; i < obstacles.size(); i++){
			if ((p1first> obstacles[i].minFirst) && (p1first < obstacles[i].maxFirst) && (p1second > obstacles[i].minSecond) && (p1second < obstacles[i].maxSecond)){
				ok = false;	
			}
			if ((p2first> obstacles[i].minFirst) && (p2first < obstacles[i].maxFirst) && (p2second > obstacles[i].minSecond) && (p2second < obstacles[i].maxSecond)){
				ok = false;	
			}
		}
		if (ok){
			Matrix<double> solution(2,2);
			solution.set(0, 0, p1first);
			solution.set(0, 1, p1second);
			solution.set(1, 0, p2first);
			solution.set(1, 1, p2second);
 			wfle.evaluate(&solution);
			evaluations++;
			f1 = wfle.getTurbineFitnesses()->get(0, 0);
			f2 = wfle.getTurbineFitnesses()->get(1, 0);
			double ct  = 750000;
			double cs  = 8000000;
			double m   = 30;
			double r   = 0.03;
			double y   = 20;
			double com = 20000;
			int n = 2;
   		double P = (
				((ct*n+cs*std::floor(n/m))*(0.666667+0.333333*std::exp(-0.00174*n*n))+com*n)/ 
					((1.0-std::pow(1.0+r, -y))/r)/(wfle.getEnergyCost() - 0.1/n)/(8760.0)
				);
				wakeFreeEnergy = P / (f1 + f2);
				wakeFreeCalculated = true;
			//}
			return;
		}
	}
}

TurbineEstimator::TurbineEstimator(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector<Obstacle> &obstacles){
	evaluations = 0;
	double centerX = wsc.width / 2;
	double centerY = wsc.height / 2;
	this->R = wsc.R;
	Matrix<double> solution(2,2);
	solution.set(0, 0, centerX);
	solution.set(0, 1, centerY);
	
	wakeFreeCalculated = false;
	for (int degree = 0; degree < 360; degree++){
		double rad = (double)(degree / 2.0) * M_PI / 180.0;
		calculateFitness(cos(rad) * 8 * (R + EPSILON), sin(rad) * 8 * (R + EPSILON), fitnessR[(degree + 180 * 2)], fitnessR[degree], obstacles, wsc, wfle);
	}
	for (int degree = 0; degree < 360 / 2; degree++){
		double rad = (double)(degree) * M_PI / 180.0;
		calculateFitness(cos(rad) * 8 * (1.5 * R), sin(rad) * 8 * (1.5 * R), fitness1_5R[(degree + 180)], fitness1_5R[degree], obstacles, wsc, wfle);
		calculateFitness(cos(rad) * 8 * (2 * R), sin(rad) * 8 * (2 * R), fitness2R[(degree + 180)], fitness2R[degree], obstacles, wsc, wfle);
		calculateFitness(cos(rad) * 8 * (2.5 * R), sin(rad) * 8 * (2.5 * R), fitness2_5R[(degree + 180)], fitness2_5R[degree], obstacles, wsc, wfle);
		calculateFitness(cos(rad) * 8 * (3 * R), sin(rad) * 8 * (3 * R), fitness3R[(degree + 180)], fitness3R[degree], obstacles, wsc, wfle);
		calculateFitness(cos(rad) * 8 * (3.5 * R), sin(rad) * 8 * (3.5 * R), fitness3_5R[(degree + 180)], fitness3_5R[degree], obstacles, wsc, wfle);
	}
}

double TurbineEstimator::estimate(double distance, double angle){
	int ang = angle;
	if (ang == 360){
		ang = 0;
	}
	if (ang < 0 || ang >= 360){
		cerr << "Error interno: " << ang << endl;
	}
	if (distance < 8 * R + EPSILON){
		return -100 - (8 * R + EPSILON - distance) * (8 * R + EPSILON - distance);
	}
	if (distance > 4.5 * 8 * R){
		return 1;
	}
	double ratioLeft = 1 - (angle - floor(angle));
	double ratioRight = 1 - ratioLeft;

	if (distance < 1.5 * 8 * R){
		int angCoverted = angle * 2;
		double ratioLeftConverted = 1 - (angle * 2 - floor(angle * 2));
		double ratioRightConverted = 1 - ratioLeftConverted;
		return min(1.0, max(0.0, fitnessR[angCoverted] * ratioLeftConverted + fitnessR[(angCoverted+1)%720] * ratioRightConverted + (fitness1_5R[ang] * ratioLeft + fitness1_5R[(ang+1)%360] * ratioRight - fitnessR[ang] * ratioLeft - fitnessR[(ang+1)%720] * ratioRight) / 0.5 / (8 * R) * (distance - (8 * R))));//Interpolation between R and 2R
	} else if (distance < 2.0 * 8 * R){
		return min(1.0, max(0.0, fitness1_5R[ang] * ratioLeft + fitness1_5R[(ang+1)%360] * ratioRight + (fitness2R[ang] * ratioLeft + fitness2R[(ang+1)%360] * ratioRight - fitness1_5R[ang] * ratioLeft - fitness1_5R[(ang+1)%360] * ratioRight) / 0.5 / (8 * R) * (distance - (1.5 * 8 * R))));//Interpolation between R and 2R
	} else if (distance < 2.5 * 8 * R){
		return min(1.0, max(0.0, fitness2R[ang] * ratioLeft + fitness2R[(ang+1)%360] * ratioRight + (fitness2_5R[ang] * ratioLeft + fitness2_5R[(ang+1)%360] * ratioRight - fitness2R[ang] * ratioLeft - fitness2R[(ang+1)%360] * ratioRight) / 0.5 / (8 * R) * (distance - (2 * 8 * R))));//Interpolation between R and 2R
	} else if (distance < 3.0 * 8 * R){//Interpolation between 2R and 3R
		return min(1.0, max(0.0, fitness2_5R[ang] * ratioLeft + fitness2_5R[(ang+1)%360] * ratioRight + (fitness3R[ang] * ratioLeft + fitness3R[(ang+1)%360] * ratioRight - fitness2_5R[ang] * ratioLeft - fitness2_5R[(ang+1)%360] * ratioRight) / 0.5 / (8 * R) * (distance - (2.5 * 8 * R))));//Interpolation between R and 2R
	} else if (distance < 3.5 * 8 * R){
		return min(1.0, max(0.0, fitness3R[ang] * ratioLeft + fitness3R[(ang+1)%360] * ratioRight + (fitness3_5R[ang] * ratioLeft + fitness3_5R[(ang+1)%360] * ratioRight - fitness3R[ang] * ratioLeft - fitness3R[(ang+1)%360] * ratioRight) / 0.5 / (8 * R) * (distance - (3 * 8 * R))));//Interpolation between R and 2R
	} else {
		return min(1.0, max(0.0, fitness3_5R[ang] * ratioLeft + fitness3_5R[(ang+1)%360] * ratioRight + (1 - fitness3_5R[ang] * ratioLeft - fitness3_5R[(ang+1)%360] * ratioRight) / 1 / (8 * R) * (distance - (3.5 * 8 * R))));//Interpolation between R and 2R
	}
}
