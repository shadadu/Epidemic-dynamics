/*
This function picks the node for a link creation based on its relative payoff. 
*/


#include <iostream>
#include "node.h"
#include "Link.h"
#include "computePayoffs.h"
#include "ran2.h"
#include <cmath>
#include "chk_payoff.h"

using namespace std;

int PickP(int numberbenefits, int payoffstate, double *& ben_arr, float r1,
		float r5, int **& G, int ***& Q, double totalpayoff, double c_0,
		int d) {

	int jj, l;
	int nbrNodes = 1000;    // The number of neighbors or degree of nodes are typically far less than nbrNodes

	double ptotal = 0.0; 

	int j, k;

	for (j = 0; j < numberbenefits; j++) {
		for (k = 0; k < nbrNodes; k++) {

			ptotal = ptotal
					+ ((G[j][k] * (ben_arr[j] / double(pow(c_0 * k, d) + 1)))
							/ double(totalpayoff)); // Running sum to pick the degree of the reacting node

			/* Uniformly, randomly pick a node from those having that degree */
			if (ptotal > r1) {
				int node1G = int(G[j][k] * r5);
				int vertex1 = Q[j][k][node1G];
				// 

				return vertex1;
			}

		}

	}

}
