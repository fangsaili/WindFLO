/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description

	Functions that try to improve the current solution (turbines)
*************************************************************************************/

#ifndef __LOCAL_SEARCH_H__
#define __LOCAL_SEARCH_H__

#include "SolutionSimpleModel.h"
#include "Obstacle.h"

/*
Do local searches by moving a set of turbines simultaneously. The move size is
stepsize.
The turbines that are moved simultaneously are distant, so that they dont 
influence each other
*/
double applyLocalSearchRealEvaluator(vector < pair <double, double> > &turbines, int width, int height, double stepSize, int remEvaluations, double R, KusiakLayoutEvaluator &wfle, vector < Obstacle > &obstacles);

/*
Try to improve by adding solutions near the boundaries of the obstacles
The reason to use this is that with the romboid representation and obstacles, some big holes might appear
*/
double applySearchNearObstacles(vector < pair <double, double> > &turbines, int width, int height, int remEvaluations, int &evaluations, double R, KusiakLayoutEvaluator &wfle, vector < Obstacle > &obstacles);
#endif
