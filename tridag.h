#include <cstdlib>
#include <cstdio>
#include <vector>

#ifndef TRIDAG_H
#define TRIDAG_H

void tridag(vector<Node*>& nodes);

void tridag(vector<Node*>& nodes)
{
	int j;
	double bet;

	int n=nodes.size();
	vector<double> gam(n);
	if (nodes[0]->d2==0.0) 
	  cerr << "Error 1 in tridag";
	
	nodes[0]->vNew=nodes[0]->r/(bet=nodes[0]->d2);
	for(j=1;j<n;j++) {
	  gam[j]=nodes[j-1]->d3/bet;
	  bet=nodes[j]->d2-nodes[j]->d1*gam[j];
	  if(bet == 0.0) 
	    cerr << "Error 2 in tridag";
	  nodes[j]->vNew=(nodes[j]->r-nodes[j]->d1*nodes[j-1]->vNew)/bet;
	}
	for(j=(n-2);j>=0;j--){
	  nodes[j]->vNew -= gam[j+1]*nodes[j+1]->vNew;
	}
}

#endif
