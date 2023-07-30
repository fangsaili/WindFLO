/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
 
 Set of functions to get the energy and fitness of a given solution, both
 with the original KusiakLayoutEvaluator (or server), as well as with the 
 surrogated model
**************************************************************************************/

#ifndef __FITNESS_CALCULATION_H_
#define __FITNESS_CALCULATION_H_

#include <vector>

#include "KusiakLayoutEvaluator.h"
#include "WindScenario.h"
#include "SolutionSimpleModel.h"

using namespace std;

void getDataInReal(vector< pair<double, double> > &turbines, KusiakLayoutEvaluator &wfle, double &energy, double &fitness);
void getDataInSimplified(SolutionSimpleModel &b, WindScenario &scenario, double &energy, double &fitness, double wakeFreeEnergy);

#endif
