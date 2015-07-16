#include <iostream>
#include "node.h"
#include "Link.h"
#include "inf_update.h"

using namespace std;

int inf_update(int xx, int c_or_b, int **& Sm, int *& Sn, node**& nd,Link**& lnk, int & sInbrs) {
	

  int nou_end_Inbrs, delta_node_indx, delta_node, delta_node_Sm_indx;
  int n, rs;

	int fstate = nd[xx]->get_infstate();

	int end_Sm_indx = nd[xx]->get_Sm_indx();
	int end_Inbrs = nd[xx]->get_Inbrs();

	    if(Sn[end_Inbrs]!=0){
	    delta_node_indx = Sn[end_Inbrs] - 1;
	    delta_node = Sm[end_Inbrs][delta_node_indx];
	    delta_node_Sm_indx = nd[delta_node]->get_Sm_indx();
	    }
	       	          		    
	nou_end_Inbrs = end_Inbrs;
	sInbrs = sInbrs - Sn[end_Inbrs] * end_Inbrs;
       

	if (c_or_b==0)      //creation
	{
		Sm[end_Inbrs][end_Sm_indx] = delta_node;
		nd[delta_node]->set_Sm_indx(end_Sm_indx);
		Sm[end_Inbrs][Sn[end_Inbrs] - 1] = -50;
		Sn[end_Inbrs]--;

		nou_end_Inbrs = end_Inbrs + 1;
			
	}
                                             
	if (c_or_b == 1) //deletion
	{
		Sm[end_Inbrs][end_Sm_indx] = delta_node;
		nd[delta_node]->set_Sm_indx(end_Sm_indx);
		Sm[end_Inbrs][Sn[end_Inbrs] - 1] = -95;
		Sn[end_Inbrs]--;
		nou_end_Inbrs = end_Inbrs - 1;
	       	
	}

	sInbrs = sInbrs + Sn[end_Inbrs] * end_Inbrs;

	sInbrs = sInbrs - Sn[nou_end_Inbrs] * nou_end_Inbrs;

	Sm[nou_end_Inbrs][Sn[nou_end_Inbrs]] = xx;
	nd[xx]->set_Sm_indx(Sn[nou_end_Inbrs]);
	nd[xx]->set_Inbrs(nou_end_Inbrs);

	Sn[nou_end_Inbrs]++;
	sInbrs = sInbrs + Sn[nou_end_Inbrs] * nou_end_Inbrs;
		  
	return 0;

}
