/************************************************************************************
 Authors
 - Carlos Segura Gonzalez
 - Manuel Guillermo Lopez Buenfil
 - Mario Alberto Ocampo Pineda
 - Sergio Ivvan Valdez
 - Salvador Botello Rionda
 - Arturo Hernandez Aguirre
*************************************************************************************/


#include <stdlib.h>
#include <vector>

#include "utils.h"

double z2, lastMean, lastDesv;
bool z2Generated = false;

double generateRandomNormal(double mean, double desv){
	double v1, v2, w, z1;
	if ((z2Generated) && (mean == lastMean) && (desv == lastDesv)){/* use value from previous call */
		z1 = z2;
		z2Generated = false;
	} else {
			do {
				v1 = ((2.0*rand()/(RAND_MAX))) - 1.0;
				v2 = ((2.0*rand()/(RAND_MAX))) - 1.0;
				w = v1 * v1 + v2 * v2;
			} while (( w >= 1.0 ) || (w == 0));

			w = sqrt( (-2.0 * log( w ) ) / w );
			z1 = v1 * w;
			z2 = v2 * w;
			z2Generated = true;
		}
	lastMean = mean;
	lastDesv = desv;
	return( mean + z1 * desv );
}
