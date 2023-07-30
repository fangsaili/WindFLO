/****************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre

 Description
 	Functions related with random numbers

******************************************************************************************/


#ifndef __UTILS_H__
#define __UTILS_H__

#include "SolutionSimpleModel.h"

static inline int getRandomInteger0_N(int n){ 
  return (int) ((n + 1.0)*rand()/(RAND_MAX+1.0));
}

static inline double generateRandomDouble0_Max(double maxValue){
	return (double)(rand()) / RAND_MAX * maxValue;
}

double generateRandomNormal(double mean, double desv);
#endif
