/*
 * Copyright 2012, 2013 Fabian Ebner
 * The program is licensed under the GPL version 3 or (at your opinion) any later version, see the file COPYING for details.
 */

#include "compare.h"

#include <math.h>
#include <stdlib.h>

double compare(mfcc_frame *mfcc_frames1, unsigned int n1, mfcc_frame *mfcc_frames2, unsigned int n2)
{
	double distances[n1 + 1][n2 + 1];
	unsigned int i = 0, j = 0, k = 0;

	//Berechne die lokalen Distanzen aller Paare von MFCC-Merkmalen
	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < n2; j++)
		{
			distances[i + 1][j + 1] = 0;
			for (k = 0; k < N_MFCC; k++)
				distances[i + 1][j + 1] += pow(mfcc_frames1[i].features[k] - mfcc_frames2[j].features[k], 2);
			distances[i + 1][j + 1] = sqrt(distances[i + 1][j + 1]);
		}
	}

	//Fill the edges with infinity
	for (i = 0; i <= n1; i++)
		distances[i][0] = atof("Inf");
	for (i = 0; i <= n2; i++)
		distances[0][i] = atof("Inf");
	//The only valid starting point on the edge
	distances[0][0] = 0;

	//Calculate the best way from one end of the matrix to the other
	for (i = 1; i <= n1; i++)
		for (j = 1; j <= n2; j++)
		{
			//Determine the cheapest predecessor
			double prev_min = distances[i - 1][j];
			if (distances[i - 1][j - 1] < prev_min)
				prev_min = distances[i - 1][j - 1];
			if (distances[i][j - 1] < prev_min)
				prev_min = distances[i][j - 1];
			//Take the step out
			distances[i][j] += prev_min;
		}

	//Normalize the distance
	return distances[n1][n2] / sqrt(pow(n1, 2) + pow(n2, 2));
}
