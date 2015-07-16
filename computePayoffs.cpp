#include <iostream>
#include "node.h"
#include "Link.h"
#include "computePayoffs.h"
#include <cmath>
#include "chk_payoff.h"

using namespace std;

int computePayoffs(int vertex, int numberPayoffs, int N, int E, int**& G,
		node **& nd, Link ** &lnk, int***& Q, int b_or_c, double c_0, int d) {

	int k, tu, n = 0;
	
	double payoff = (nd[vertex]->get_benefit())
			/ (double((pow((nd[vertex]->get_deg()) * c_0, d) + 1)));

	nd[vertex]->set_payoff(payoff);
	int old_deg = -10;
	int deg = nd[vertex]->get_deg();
	int old_Q_indx = nd[vertex]->get_Q_indx();
	int ben_dex = nd[vertex]->get_ben_indx();
	int Q_indx, Qnodes, movNode;
	int Qnodes2 = G[ben_dex][deg];

	if (b_or_c == 1) {
		old_deg = deg - 1;
		Qnodes = G[ben_dex][old_deg];
		movNode = Q[ben_dex][old_deg][Qnodes - 1];
		Q[ben_dex][old_deg][old_Q_indx] = movNode;
		int movNode1 = movNode;
		Q[ben_dex][old_deg][Qnodes - 1] = -21;
		nd[movNode]->set_Q_indx(old_Q_indx);
		G[ben_dex][old_deg]--;
	}
	if (b_or_c == 0) {
		old_deg = deg + 1;
		Qnodes = G[ben_dex][old_deg];
		movNode = Q[ben_dex][old_deg][Qnodes - 1];
		Q[ben_dex][old_deg][old_Q_indx] = movNode; //
		Q[ben_dex][old_deg][Qnodes - 1] = -11;
		nd[movNode]->set_Q_indx(old_Q_indx);
		G[ben_dex][old_deg]--;
	}

	nd[vertex]->set_Q_indx(Qnodes2);
	Q[ben_dex][deg][Qnodes2] = vertex;
	G[ben_dex][deg]++;

	return 0;

}
