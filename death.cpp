#include <iostream>
#include "node.h"
#include "Link.h"
#include "death.h"
#include "brk_nodes.h"
#include "computePayoffs.h"
#include "update_nbrs.h"
#include "update_dead_payoff.h"
#include "update_nbr_payoff.h"
#include "move_deadnode.h"
#include "rmlink_infupdate.h"
#include <fstream>

using namespace std;

int death(int deadnode, int lastnode, node **& nd, Link **& lnk,int numberNodes, int numberEdges, int numberLinks, double c_0, int d,double *& ben_arr, int **& G, int***& Q, int numberPayoffs, int**& Sm,int *&Sn, int*& Inodes, int &sInbrs, int &nInfected) {
  
    int deg, i, broken, maxDeg, vtx;
    int b_or_c = 0;
	maxDeg = 2000;
	deg = nd[deadnode]->get_deg();
	int links_removed = 0;
	int *temp_nbr_arr;
	temp_nbr_arr = new int[deg];
	int deadnode_status = nd[deadnode]->get_infstate();
	for (i = 0; i < deg; i++) {
		temp_nbr_arr[i] = nd[deadnode]->get_nbr(i); //holds the neighbors of the node to be killed here
	}
	for (i = 0; i < deg; i++) //this loop detaches deadnode from all its nbrs, i.e. makes it a zero degree node
			{
		int broken = brk_nodes(deadnode, temp_nbr_arr[i], nd, lnk, numberLinks);
		int b_or_c = 0;
		vtx = temp_nbr_arr[i];					
		int payoffstate = computePayoffs(vtx, numberPayoffs, numberNodes,numberEdges, G, nd, lnk, Q, b_or_c, c_0, d);
		payoffstate = computePayoffs(deadnode, numberPayoffs, numberNodes,numberEdges, G, nd, lnk, Q, b_or_c, c_0, d);
		lnk[broken]->setEdges(-1, -1);
		rmlink_infupdate(deadnode, vtx, numberLinks, nd, lnk, Sm, Sn, sInbrs);
		links_removed++;
		numberLinks--;
	}
	
	int deadnode_sicknbrs = nd[deadnode]->get_Inbrs();
	int deadnode_Sm_indx = nd[deadnode]->get_Sm_indx();
	int updating_nbrs = update_nbrs(lastnode, deadnode, nd, lnk);
	int movdeadnode = move_deadnode(deadnode, lastnode, maxDeg, ben_arr, nd,lnk, G, Q, Sm, Sn, Inodes, nInfected);	
	delete[] temp_nbr_arr;

	return numberLinks;

}

