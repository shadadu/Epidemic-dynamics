/*
A simulation of social network and infection dynamics for the study of how human behavior affects disease transmission. 
This function (hivdynamics.cpp) is the heart and workhorse of the simulation. It can be broken into 6 main processes:
Link creation, link breaking, 

NB: This code was written in building blocks according to increasing complexity of the model. i.e., there initially were
only the link creation and breaking processes. The dynamics of these 2 processes were then intensively studied.
Birth and death processes were then added, and the interactions of the now 4 processes studied. Finally, infection
transmission and death from infection were added.

The code was built this way to ensure that a full understanding of the basic model before adding
more complexity in terms of extra processes. Thus, the build-up from 2 processes through 4 to 6.
*/

#include <iostream>
#include "node.h"
#include "Link.h"
#include <cmath>
#include <sstream>
#include "ran2.h"
#include "death.h"
#include "brk_nodes.h"
#include "computePayoffs.h"
#include "chk_payoff.h"
#include "PickP.h"
#include "pkp.h"
#include "update_nbrs.h"
#include "infect.h"
#include "inf_update.h"
#include "infect_tx.h"
#include "crtlink_infupdate.h"
#include "rmlink_infupdate.h"
#include "death_infupdate.h"
#include "picknewI.h"
#include "adaptive_fxn.h"
#include "doubleToStr.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;



int hivdynamics(double k0, int N, int E, int numberLinks, node **& nd,Link **& lnk, double alpha, double beta, int stp, 
	double*& I_star, double inf_rate, int nbr_infs, double *& N_star, double *& k_star, int maxMCS, int nouMCS, double inf_death_rate) {

  int i, j, k, ii, n, jj, l, MCS, numberPayoffs, newborn, ndead, linkchange, Popn, suscepts;
  int node1, node2, end1, end2;
  int factor=1;
  double kmax=k0;
	double t = 0.00;
	double u, dt, avdeg;
	double avdeg;
	int vertex = -1;
	double H, L;
	double C, B, R, rate1, RT, R3, R4, R5, R6, totalpayoff, brktotalpayoff;
	int nbrNodes = 1000, maxEdges=E, mxdeg=1000, maxDeg=1000, Init_Infected=500;
	int hi_benefits = 0, lo_benefits=0, hi_suscepts=0, lo_suscepts=0, hi_infected=0, lo_infected=0;
	int mxej = maxEdges, mcsStart=500000;
    double birth_rate, death_rate, infection_rate, infection_death;
	Popn = N;
	suscepts = Popn;
	int process=-1;

    MCS = 0;
	int breaking = 0, create=0, brak=0, mcs=0;
	/*
	The following integer arrays are to store the time series of infection prevalence according to benefit values. i.e., the fraction infected
	that are high or low benefits. The arrays are initialized to zero integers.
	*/
	int* Pop_time = new int[nouMCS];
	int * ts_lo_infected = new int[nouMCS];
	int * ts_lo_susceptibles =  new int[nouMCS];
	int * ts_hi_infected = new int[nouMCS];
	int * ts_hi_susceptibles = new int[nouMCS];
	
	for (n = 0; n < nouMCS; n++){  // Initializing the arrays
		Pop_time[n] = 0;
		ts_lo_infected[n]=0;
		ts_hi_infected[n]=0;
		ts_lo_susceptibles[n]=0;
		ts_hi_susceptibles[n]=0;
	}

	double* av_deg = new double[nouMCS];  // Time series for the average degree of the network
	for (n = 0; n < nouMCS; n++)
		av_deg[n] = 0;

	int* sumdgdstr = new int[maxEdges];
	int* sepdeg = new int[maxEdges];
	int* sepdeg2 = new int[maxEdges];
	for (n = 0; n < maxEdges; n++) {
		sumdgdstr[n] = 0;
		sepdeg[n] = 0;
		sepdeg2[n] = 0;
	}


	H = 1.0;
	L = 0.5;
	int numberbenefits = 2;
	int *b = new int[numberbenefits];
	numberPayoffs = numberbenefits * mxdeg;


	double* payoffdistr = new double[numberPayoffs];
	for (n = 0; n < numberPayoffs; n++)
		payoffdistr[n] = 0.0;

	double* payoffdistr2 = new double[numberPayoffs];
	for (n = 0; n < numberPayoffs; n++)
		payoffdistr2[n] = 0.0;


	double* infection_rate_arr = new double[nouMCS];
	for (n = 0; n < nouMCS; n++) {
		infection_rate_arr[n] = 0.0;
	}

	

	int zize = 2 * N;
	int* Inodes = new int[zize];
	for (n = 0; n < zize; n++) {
		Inodes[n] = -10;
		
	}

	int lngth = 2*Popn;
	int *Sn;   // dimensions 1 x mxdeg 
	int **Sm;  // dimensions mxdeg x N, but length of Sm[0][2*N] to make up for when we begin with large number of nodes with zero degree
	Sn = new int[mxdeg];
	Sm = new int*[mxdeg];
	for (j = 0; j < mxdeg; j++) 
        {
		Sm[j] = new int[zize];
		Sn[j] = 0;
		for (n = 0; n < zize; n++) {
			Sm[j][n] = -5;
		}
	}


	
	int sInbrs = 0;               // Keeps track of the number of infected nodes.
	int * const sum_Inbrs = &sInbrs;   //Pointer the the variable for the number of infected nodes
	int number_Inbrs = 0;

	int nInfected = 0;             // The number of infected nodes and a pointer to it. The pointer is useful so the variable can be passed to functions
	int * const number_Infected = &nInfected;

	int ndead_inf = 0;

	int payoffstate = 0; //the number of unique payoff values computed at a particular time point
	int **G; //G[benefit][degree]= # of nodes with these benefit and degree values, i.e., a 2-D matrix whose elements are the # of nodes with corresponding 
			 //benefit and degree values

	G = new int*[numberbenefits]; //the j-deg element here stores the number of nodes with benefit index j and degree deg

	for (j = 0; j < numberbenefits; j++) {
		G[j] = new int[mxdeg];

		for (int deg = 0; deg < mxdeg; deg++) {
			G[j][deg] = 0;
		}
	}

	int ***Q = new int**[numberbenefits]; //Q[benefit][degree][node index]: the elements will be the nodes(i.e their index: 0...N-1) with their corresponding benefit and degree values
	for (j = 0; j < numberbenefits; j++) {
		Q[j] = new int*[mxdeg];
		for (int deg = 0; deg < mxdeg; deg++) {
			Q[j][deg] = new int[N];
			for (i = 0; i < N; i++)
			      Q[j][deg][i] = -1; //initialize the Q-matrix elements to -1; real nodes are indexed from 0 to N-1; initialization to -1 serves as a check  

		}
	}

	int* dgdstr = new int[maxEdges];
	for (n = 0; n < maxEdges; n++)
		dgdstr[n] = 0;

	double *tyme = new double[nouMCS]; //stores the real number time values at each reaction or "monte carlo" step
	double *av_poff = new double[nouMCS]; //stores the average payoff over the entire network at each reaction or "monte carlo" step
	for (n = 0; n < nouMCS; n++) {
		tyme[n] = 0.00;
		av_poff[n] = 0.0;
	}

	int d = 1;
	double c_0;

	/*

	*/
	double * Infected_ts = new double[nouMCS];
	int * infected_number_series = new int[nouMCS];
	double * changet = new double[nouMCS];
	for (n = 0; n < nouMCS; n++) {
		changet[n] = 0.00;
		Infected_ts[n] = 0.0;
		infected_number_series[n] = 0;
	}

	long gg = -100;
	long *idum;
	idum = &gg;

	//assignment of inherent benefit values to each node; index for benefit H is 0, and index for benefit L is 1
	float fraction_high = .5;
	for (i = 0; i < Popn; i++) {
		float r = ran2(idum);
		if (r < fraction_high) {
			nd[i]->set_benefit(H, 0);
			hi_benefits++;
		} else {
			nd[i]->set_benefit(L, 1);
			lo_benefits++;	
		}
	}
            
   	
	int b_or_c = -1; //in the computePayoffs function, there is the need to know if link was created or broken. b_or_c=-1 indicates that neither occurred
	int c_or_b = -1; //we initialize all nodes to be susceptibles

	for (ii = 0; ii < Popn; ii++) //updates payoffs: initializes payoffs/initializes infection replace 10 with Popn
			{
		b_or_c = -1;
		c_or_b = -1;
		vertex = ii;
		payoffstate = computePayoffs(vertex, numberPayoffs, Popn, maxEdges, G,nd, lnk, Q, b_or_c, c_0, d);
		inf_update(vertex, c_or_b, Sm, Sn, nd, lnk, sInbrs);
		int nd_Sm_indx = nd[vertex]->get_Sm_indx();
		 
	}

	
	ii=0;
	while(ii<Init_Infected)   // This loop initializes the number of infected. It uniformly, randomly picks Init_Infected nodes and changes their state from
	  {                       // susceptible to infected. The rest of the loop is book-keeping: an infected node must be added to 

	    float sick_r=ran2(idum);  // Generate uniform random number

	    int sick=int(Popn*sick_r);  // Uniformly, randomly pick an node to be infected. 

	    int nd_infstate=nd[sick]->get_infstate();  

	    if(nd_infstate==0 )     // If the node we picked has been selected earlier in this loop, then it is infected. We need not re-infect it then. Only uninfected
			{                   // nodes get infected.   
		
		    infect_tx(sick,nd,lnk,Sm,Sn,sInbrs);   // The node is infected. Number of infected nodes, and the number of infected neighbors are passed
		                                           // to infect_tx to be updated

		    Inodes[nInfected] = sick;              // Add it to the infected nodes array
			nd[sick]->set_Inodes_indx(nInfected);  // Update it's infection status
			nInfected++;                          
		    suscepts--;
		    ii++;
	       }
	  }



	double *ben_arr = new double[numberbenefits]; //this array also can refer to benefit value from the benefit-index; each benefit index has its corresponding benefit value in this array
	ben_arr[0] = 1.0;
	ben_arr[1] = 0.5;
	
	int brk = 0;
	int cnt = 0;
	int cntr = 0;

	float *rxn = new float[nouMCS];
	for (n = 0; n < nouMCS; n++) {
		rxn[n] = 0.0;
	}
 	
	newborn = 0;
	ndead = 0;
	linkchange = 0;
	
	sInbrs = 0;
	for (n = 0; n < 100; n++) {      // Sums up the number of infected neighbor over the network. Nodes have been grouped according to the number
	   sInbrs = sInbrs + Sn[n] * n;  // We set the maximum for the number of infected to be 100. Given that we expect degrees nodes typically to
	}                                // be less than 20, it is safe to have a maximum of 100 without running into array out of bounds, segmentation faults.

       
    c_0 = 1 / double(k0);        // Parameter k0 is risk tolerance
		
	for (MCS = 0; MCS < maxMCS; MCS++) {
	             		
		totalpayoff = 0.0;           //initializes the link creation rate to zero...totalpayoff here corresponds to sum(payoffs) over all nodes
		brktotalpayoff = 0.0;        //initializes link breaking rate to zero... corresponds to sum((1-payoff)*degree) over all nodes 

		for (j = 0; j < numberbenefits; j++) //this loop computes the creation and breaking rates 
				{
			for (k = 0; k < nbrNodes; k++) {
				totalpayoff = totalpayoff
						+ G[j][k] * (ben_arr[j] / double(pow(c_0 * k, d) + 1));  // running sum of creation rates over benefit-degree matrix
				brktotalpayoff = brktotalpayoff + k * G[j][k]* (1 - (ben_arr[j]/ double(pow(c_0 * k, d) + 1))); // running sum of breaking rates over benefit-degree matrix
			}
		}
		
		C = .5 * (1 / double(Popn)) * pow(totalpayoff, 2.00);   // Computes the total link creation rate over the network
		B = (brktotalpayoff);                                   // The total link breaking rate over the network    
		
		R = alpha * C + beta * B;                               // Total for link creation and breaking rates which are respectively multiplied by the link
		                                                        // creation and link breaking factors alpha and beta respectively.

		double cnctRate = (alpha * C) / R;                      // The relative rate for link creation or link breaking to occur
		double brkRate = (beta * B) / R;

		b_or_c = -2;

		birth_rate = 50;
		death_rate = 1/double(100);                            
		infection_rate = inf_rate;

		infection_death = inf_death_rate;

		R3 = birth_rate;    //                                 

		R4 = death_rate * Popn;

		R5 = infection_rate * sInbrs;
		
		R6 = infection_death * nInfected;

	      

		RT = R + R3 + R4 + R5 + R6;             // Total reaction rate for the full system of 6 processes or reactions

		float r_m = ran2(idum);

		rate1 = ran2(idum);

		if (r_m < R / float(RT)) //link create/break loop begins here
				{
			linkchange++;

			if (rate1 < (alpha * C / R)) {

			        process=1;
			  
				
				create++;
				b_or_c = 1;  //to indicate that we are creating link here; function computePayoffs which updates the node payoff values and moves nodes to appropriate position in Q, etc, needs to know
				float r1 = ran2(idum);
				float r5 = ran2(idum);
				node1 = PickP(numberbenefits, payoffstate, ben_arr, r1, r5, G, Q, totalpayoff, c_0, d);

				node2 = PickP(numberbenefits, payoffstate, ben_arr, r1, r5, G, Q, totalpayoff, c_0, d);

				while (nd[node1]->chk_lnk(node2) == 0) //this loop for eliminating double and self links
				{
					r1 = ran2(idum);
					r5 = ran2(idum);

					cntr++;
					node1 = PickP(numberbenefits, payoffstate, ben_arr, r1, r5, G, Q, totalpayoff, c_0, d);

					r1 = ran2(idum);
					r5 = ran2(idum);
					node2 = PickP(numberbenefits, payoffstate, ben_arr, r1, r5, G, Q, totalpayoff, c_0, d);
				}

				
				if (numberLinks + 1 > maxEdges) //if loop: create a larger link array to store the new larger number of links
						{
					Link** temp;
					temp = new Link*[2 * maxEdges];
					for (l = 0; l < maxEdges; l++) {
						temp[l] = lnk[l];
					}
					for (l = maxEdges; l < 2 * maxEdges; l++)
						temp[l] = new Link();

					delete[] lnk;

					lnk = temp;
					maxEdges = 2 * maxEdges;
				}

			       
				lnk[numberLinks]->setEdges(node1, node2); //set the nodes this link is attached to; tells the link what nodes it connects 

				nd[node1]->add_nbr(node2, numberLinks);   
				nd[node2]->add_nbr(node1, numberLinks);

				payoffstate = computePayoffs(node1, numberPayoffs, Popn,maxEdges, G, nd, lnk, Q, b_or_c, c_0, d);  // Updates payoff of first node

				payoffstate = computePayoffs(node2, numberPayoffs, Popn,maxEdges, G, nd, lnk, Q, b_or_c, c_0, d);

				crtlink_infupdate(node1, node2, numberLinks, nd, lnk, Sm, Sn,sInbrs);          //updates infection arrays and variables; book-keeping to put nodes 

				numberLinks++;
				
				lnk[numberLinks - 1]->set_crt_time(t); //store the time this link was created. This so as to keep track of link turnover or lifespan of each link, 
				// i.e. how long before this link is broken again;  each created link is treated as being unique

				
				
			}

			if (rate1 >= (alpha * C / R))     //break link loop begins here
					{
					  
					  //	 	  cout<<"BREAK"<<endl;
				process=2;
				brak++;
				b_or_c = 0;  // indicates that link is being broken
				float r2 = ran2(idum);
				float r4 = ran2(idum);
				end1 = pkp(payoffstate, numberbenefits, r2, r4, nd, Q, G, ben_arr, brktotalpayoff, c_0);
				int deg = nd[end1]->get_deg();
				
				int end2indx = int(deg * ran2(idum)); //picks one of the neighbors of end1 to break from. end2indx only indicates the position of the neighbor picked thus. 
													  //it's index must be read from the neighbor array of end1
				end2 = nd[end1]->get_nbr(end2indx); // gets the neighbor of end1 to be broken from, i.e. its index

				int deg2 = nd[end2]->get_deg();
				int ben2 = nd[end2]->get_ben_indx();
				
				
				int edge = brk_nodes(end1, end2, nd, lnk, numberLinks); //breaks the link between end1 and end2, and returns the index of the link broken here as edge
							
				int end1_Inbrs=nd[end1]->get_Inbrs();
				int end2_Inbrs=nd[end2]->get_Inbrs();
				
			    rmlink_infupdate(end1, end2, numberLinks, nd, lnk, Sm, Sn,sInbrs);
				
				numberLinks--;
				
				payoffstate = computePayoffs(end1, numberPayoffs, Popn,maxEdges, G, nd, lnk, Q, b_or_c, c_0, d);
				payoffstate = computePayoffs(end2, numberPayoffs, Popn,maxEdges, G, nd, lnk, Q, b_or_c, c_0, d);

				
				lnk[edge]->set_lifespan(); //stores links lifespan, or time between creation and breaking
				double span = lnk[edge]->get_lifespan();

				breaking++;

	
					  
			}    //breaking link loop ends

		}   //Combined loop for link creation and breaking ends here

		else if (r_m - R / float(RT) < R3 / float(RT))               // Natural birth process
		{
		  
			newborn++;
			
			process=3;   // For checks when needed
	
                    
			nd[Popn]->stnodeindx(Popn, maxDeg);
			int nodeindx = nd[Popn]->get_nodeindx();

			int new_Q_indx = G[0][0];
			int new_born = Popn;             // Put newborn in next available position in node array

			float new_benefit = ran2(idum);  // Generate uniform random number between 0 and 1
			/*
			Set the newborn's benefit as high or low
			*/
			if (new_benefit < fraction_high) {  
				nd[new_born]->set_benefit(H, 0);
				hi_benefits++;
				
			} else {
				nd[new_born]->set_benefit(L, 1);
				lo_benefits++;
				
			}

			int new_born_Sm_indx = Sn[0];     

			nd[Popn]->set_Q_indx(new_Q_indx);
			                   
 			nd[Popn]->set_Sm_indx(new_born_Sm_indx);
			nd[Popn]->set_Inbrs(0);

			c_or_b = -2;   // A flag to indicate that a link that neither a link breaking or creation occurred so the appropriate book-keeping can be done 

			inf_update(new_born, c_or_b, Sm, Sn, nd, lnk, sInbrs);    // Updates the payoff of new node and adds it to the array of susceptible nodes 

			int newborn_ben = nd[Popn]->get_ben_indx();
			int vertex1 = Popn;
			
			b_or_c = -2;  // Flag indicating that a link was neither added or broken so the benefit-degree matrices G and Q can be updated appropriately 

			payoffstate = computePayoffs(vertex1, numberPayoffs, Popn, maxEdges,G, nd, lnk, Q, b_or_c, c_0, d);

			Popn++;
		  
		}

		else if (r_m - (R + R3) / double(RT) < R4 / double(RT))  //natural death
		{
		  
			ndead++;
			
			process=4;

			float r_s = ran2(idum);
			int deadnode = int(Popn * r_s);
			int lastnode = Popn - 1;
		                  
			int dead_infstate = nd[deadnode]->get_infstate();

			double deadnode_benefit = nd[deadnode]->get_benefit();

			if(deadnode_benefit==1){  // Update number of high benefit or low benefit nodes depending on the benefit of dead node (book-keeping)
			  hi_benefits--;
			}
			else{
			  lo_benefits--;
			}

			/*
            Book-keeping as deadnode is removed from the system. Links to dead node are removed and the node object is re-initialized for possible later 
			as a newborn
			*/
			numberLinks = death(deadnode, lastnode, nd, lnk, N, E, numberLinks,c_0, d, ben_arr, G, Q, numberPayoffs, Sm, Sn, Inodes,sInbrs, nInfected);
			Popn--;
		                
		}

		else if (r_m - (R + R3 + R4) / double(RT) < R5 / double(RT)) //infection 
		{
		  
		       
		     process=5;
			 int risk_of_infection=0;
			 
			 float r_inf = ran2(idum);
			 float rr_2inf=ran2(idum);
			 
			 int new_I=picknewI(sInbrs,maxDeg,r_inf,rr_2inf, Sm, Sn, nd, number_Inbrs );  // Pick a node to infect from looking at the susceptibles according
			                                                                              // to their number of infected neighbors. 
			 
			 infect_tx(new_I,nd,lnk,Sm,Sn,sInbrs );          // Infect the node; book-keeping
			 

			 double newI_benefit = nd[new_I]->get_benefit();

			 Inodes[nInfected]=new_I;
			 nd[new_I]->set_Inodes_indx(nInfected);
			 nInfected++;
			 

				  	 
		}

		else if (r_m - (R + R3 + R4 + R5) / double(RT) < R6 / double(RT)) //death from infection
		{
		  
		  //cout<<"DEATH FROM INFECTION"<<endl;
                         process=6;
			 ndead_inf++;
			 ndead++;
			
			 int lastnode=Popn-1;
			 int dud_pos = int(nInfected*ran2(idum));   // Uniformly, randomly pick and infected node
			 int dud=Inodes[dud_pos];

			 double dudnode_benefit = nd[dud]->get_benefit();

			 if(dudnode_benefit==1)
			   {
			     hi_benefits--;
			   }
			 else{
			     lo_benefits--;
			     }

			
			 int lastnode_Inodes_indx=nd[lastnode]->get_Inodes_indx();
			 /* 
			 Remove dead node from system; book-keeping */
			 numberLinks = death(dud, lastnode, nd, lnk, N, E, numberLinks,c_0,d, \
			 	 ben_arr, G, Q, numberPayoffs,Sm,Sn,Inodes,sInbrs,nInfected );
			 
			 Popn--;
		  
		}




		double var_ss;


		
		u = ran2(idum);  // Uniform random number for updating time. 
		dt = -1 * (1 / R) * log(u); 
		t = t + dt;  // time update. See Gillespie algorithm

		mcs = int(MCS / double(100));  // The time series are sampled at intervals of 100 to reduce memory needs
		
		if (MCS % 100 == 0) {
			
			rxn[mcs] = R;  //stores the values of R at each reaction  step
			Pop_time[mcs] = Popn;
			
			double frac_infected=nInfected/double(Popn);

			Infected_ts[mcs]=frac_infected;
			infected_number_series[mcs] = nInfected;

			int qq;
			int dg_nodes = 0;
			double sumpff = 0.0;


			for (ii = 0; ii < Popn; ii++) {   // Take degree distribution
				dg_nodes = dg_nodes + nd[ii]->get_deg();
				sumpff = sumpff + nd[ii]->get_payoff();
			
			}

			av_deg[mcs]  = dg_nodes / double(Popn);
			av_poff[mcs] = sumpff / double(Popn);
			tyme[mcs] = t;
			

			int lo_benefit_suscepts=0;
			int hi_benefit_suscepts=0;
  		         for(j=0; j<mxdeg; j++)
			  {
			  
			    for(n=0;n<Sn[j];n++)
			      {
				    int da_node = Sm[j][n];
			
				    double da_ben = nd[da_node]->get_benefit();
				

				        if(da_ben==1)
				        {
				         hi_benefit_suscepts++;
				        }
                        else{
				        lo_benefit_suscepts++;
				        }
			      }

			  }

			ts_hi_susceptibles[mcs]=hi_benefit_suscepts;
			ts_lo_susceptibles[mcs]=lo_benefit_suscepts;


			
			int lo_benefit_infected=0;
			int hi_benefit_infected=0;
			for(n=0;n<nInfected;n++)
			  {
			    int sick_node = Inodes[n];
			    double sick_node_benefit = nd[sick_node]->get_benefit();
			    if(sick_node_benefit==1)
			      {
				hi_benefit_infected++;
			      }
			    else{
			       lo_benefit_infected++;
			    }
			  }		

			ts_hi_infected[mcs]=hi_benefit_infected;
			ts_lo_infected[mcs]=lo_benefit_infected;



	               int alldeg = 0;
	               for(j=0;j<numberbenefits;j++)
			 {
			   for(k=0;k<maxDeg;k++)
			     {
			       alldeg=alldeg+ k*G[j][k];
			     }
			 }
      

			if (mcs > mcsStart) {  // Take data after mcsStart. i.e., for these metrics, we are interested in steady state data
				for (i = 0; i < Popn; i++) { // This for loop is for taking degree distribution of the network
					int qq = nd[i]->get_deg();
					sumdgdstr[qq] = sumdgdstr[qq] + 1;
	                        int nodes_ben=nd[i]->get_benefit();
				if(nodes_ben==1){ // If condition to separately take high and low benefit degree distribution; 'if' separates nodes according to the benefit value
				  sepdeg[qq]=sepdeg[qq]+1;
				     }else{
				  sepdeg2[qq]=sepdeg2[qq]+1;			  
				    }
				}

			}
			changet[mcs] = dt;   //stores the inter-arrival times; can be observed to see if simulation is behaving as expected, without unusual values for dt
			infection_rate_arr[mcs] = sInbrs;
      		mcs++;
			//cout<<"done with loop"<<endl;

		}
		//cout<<"done with a mcs step"<<endl;	
	
	
	}   //MCS ends here

	
	cout << "Simulation run done:   "<<MCS << endl;

	double tlast;
	tlast = t;
	

	 avdeg=0.00;
	 
	 double I_ss=0.0, I_ss1=0.00, k_ss=0.0, var_s=0.0, N_ss=0.0, N_ss1=0.00, k_star_ss=0;
	 
	
	 for(mcs=mcsStart;mcs<nouMCS;mcs++)
	 {
	 avdeg = avdeg + av_deg[mcs];
	 I_ss1 = I_ss1 + Infected_ts[mcs];
	 N_ss1 = N_ss1 + Pop_time[mcs];
	 }


	 I_ss=I_ss1/double(nouMCS-mcsStart);
	 N_ss=N_ss1/double(nouMCS-mcsStart);
	 k_ss=avdeg/double(nouMCS-mcsStart);
	 

	 I_star[stp]=I_ss;
	 N_star[stp]=N_ss;
	 k_star[stp]=k_ss;

	 /*
	 Output data into text files for analysis in Matlab 
	*/
	 	  
	 ofstream output9;
	 output9.open("CumulativeDegreeDistribution");
	 for (n=0; n<numberLinks; n++) 
	 {
	 output9 <<n<<"  "<<sumdgdstr[n]<< endl;
	 }
	 output9.close();
	 
	 ofstream output10;
	 output10.open("AverageDegreeTimeSeries");
	 for(MCS=0;MCS<nouMCS;MCS++)
	 {output10<<tyme[MCS]<<" "<<av_deg[MCS]<<endl;}
	 output10.close();
	 
	 ofstream output12;
	 output12.open("PayoffTimeSeries");
	 for(n=0;n<nouMCS;n++)
	 { output12<<tyme[n]<<" "<<av_poff[n]<<endl;}
	 output12.close();


	 ofstream output13;
	 output13.open("MultipleBenefitDegreeDistribution");
	 for(n=0;n<maxEdges;n++)
	 {output13<<n<<"      "<<sumdgdstr[n]<<"         "<<sepdeg[n]<<"     "<<sepdeg2[n]<<endl;}
	 output13.close();


	 float mnDeg;
	 mnDeg = avdeg/float(nouMCS-100);
 
	 ofstream rxnrates;
	 rxnrates.open("reaction rates");
	 for(n=0;n<nouMCS;n++)
	 {
	 rxnrates<<n<<" "<<rxn[n]<<endl;
	 }
	 rxnrates.close();


	 ofstream arrival;   
	 arrival.open("arrival");
	 for(n=0;n<nouMCS;n++)
	 {
	 arrival<<n<<" "<<changet[n]<<endl;
	 }
	 arrival.close();


	 ofstream InfectionRate;
	 InfectionRate.open("InfectionRate");
	 for(n=0;n<nouMCS;n++)
	 {
	 InfectionRate<<tyme[n]<<" "<<infection_rate_arr[n]<<endl;
	 }
	 InfectionRate.close();

	 int totaldeg=0;
	 for(j=0;j<numberbenefits;j++)
	 {
	 for(k=0;k<mxdeg;k++)
	   {
	   totaldeg=totaldeg + G[j][k]*k;
	   }
	 }


	 cout<<"hello"<<endl;

	 
	 ofstream Population;
	 Population.open("Population_time_series");
	 for(n=0;n<nouMCS;n++)
	 {
	 Population<<tyme[n]<<"  "<<Pop_time[n]<<endl;
	 }
	 Population.close();


	 ofstream Infctds;
	 Infctds.open("Infected_time_series");
	 for(n=0;n<nouMCS;n++)
	 {
	 Infctds<<tyme[n]<<" "<<Infected_ts[n]<<endl;
	 }
	 Infctds.close();


	 ofstream ss_I;
	 ss_I.open("Steady_State_Infecteds");
	 for(n=0;n<nbr_infs;n++)
	 {
	 ss_I<<n<<" "<<I_star[n]<<endl;
	 }
	 ss_I.close();


	 ofstream ss_N;
	 ss_N.open("Steady_State_Population");
	 for(n=0;n<nbr_infs;n++)
	   {
	     ss_N<<n<<"  "<<N_star[n]<<endl;
	   }
	 ss_N.close();


	 ofstream ss_k;
	 ss_k.open("Steady_State_degree");
	 for(n=0;n<nbr_infs;n++)
	   {
	     ss_k<<n<<" "<<k_star[n]<<endl;
	   }
	 ss_k.close();

	
	 ofstream benefits_infected;
	 benefits_infected.open("benefitsandinfection");
	 for(n=0;n<nouMCS;n++)
	   {
	benefits_infected<<tyme[n]<<"  "<<ts_lo_infected[n]<< "  "<<ts_lo_susceptibles[n] <<"  "<<ts_hi_infected[n]<<"   "<<ts_hi_susceptibles[n]<<"   "<<infected_number_series[n]<<"   "<<Pop_time[n]<<endl;
	   }
	 benefits_infected.close();	 



	 /* 
	 // Can be used to store time series data of each run
	 ofstream TimeSeries;
	 std::string filename;
	  for(n=0;n<nbr_infs;n++)
	   {
	       filename = "InfectSeries" + doubleToStr(n) +".txt";

	       TimeSeries.open(filename.c_str());
	       
	        for(j=0;j<nouMCS;j++)
		     {
		     std::string text_here=doubleToStr(ts_matrix[n][j]);
		     TimeSeries<< text_here<<" \n";  
             TimeSeries.close();
			 }
      }
	 */


	
   /*
   Freeing up memory
   */
	delete[] Pop_time;
	delete[] av_deg;
	delete[] ts_lo_infected;
	delete[] ts_hi_infected;
	delete[] ts_lo_susceptibles;
	delete[] ts_hi_susceptibles;
	delete[] av_poff;
	
	delete[] sumdgdstr;
	delete[] sepdeg;
	delete[] sepdeg2;
	delete[] b;
	delete[] infection_rate_arr;
	delete[] Inodes;
	delete[] Sn;
	delete[] dgdstr;
	delete[] tyme;
	delete[] Infected_ts;
	delete[] infected_number_series;
	delete[] changet;
	delete[] ben_arr;
	delete[] rxn;
	delete[] payoffdistr;
	delete[] payoffdistr2;

		
	for (j = 0; j < numberbenefits; j++)
	{
	  delete[] G[j];
	}
	delete[] G;

	
	for (j =0; j < mxdeg; j++) 
        {
	   delete[] Sm[j];
	}
	delete[] Sm;

	for (j = 0; j < numberbenefits; j++)
        {
		for (n = 0; n < maxDeg; n++) 
                {
		  delete[] Q[j][n];
		}
	}

	for (j = 0; j < numberbenefits; j++) {
	      
		delete[] Q[j];
	}
	delete[] Q;


	

	return maxEdges;

}

