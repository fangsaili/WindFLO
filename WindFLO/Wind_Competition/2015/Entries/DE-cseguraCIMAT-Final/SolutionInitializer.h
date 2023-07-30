/****************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
 
 Methods used for the initialization of solutions: random solution + adaptive local search
******************************************************************************************/

#ifndef __SOLUTION_INITIALIZER__H__
#define __SOLUTION_INITIALIZER__H__

#include <vector>
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include "Obstacle.h"

using namespace std;

//Create a solution and improve it with an adaptive local search (a romboid-type solution)
void createSolutionRomboidImproved(WindScenario &wsc, KusiakLayoutEvaluator &wfle, vector < pair<double, double> > &bestTurbines, double &deg1, double &deg2, double &newdist1, double &newdist2, vector < Obstacle > &obstacles, int lastEval, bool initRandom);
//Create a romboid-type solution
bool createSolutionRomboid(double deg1, double deg2, double dist1, double dist2, WindScenario &wsc, vector < pair<double, double> > &turbines, vector < Obstacle > &obstacles);

#endif
