/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
 
 Representation of an individual as a romboid, with the internal angle, external angle 
 and sizes of the sides
**************************************************************************************/

#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__
#include <vector>

using namespace std;

struct Individual {
	double parameters[4];//ang1, ang2, d1, d2
	double fitnessReal;
	double fitnessApprox;
	vector< pair<double, double> > turbines;
};

bool cmpByApprox(const Individual &i1, const Individual &i2){
	return i1.fitnessApprox < i2.fitnessApprox;
}

bool cmpByReal(const Individual &i1, const Individual &i2){
	return i1.fitnessReal < i2.fitnessReal;
}

#endif
