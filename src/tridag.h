#include <cstdlib>
#include <cstdio>
#include <vector>
#include <memory>

#ifndef TRIDAG_H
#define TRIDAG_H
namespace LongQt {

void tridag(std::vector<std::shared_ptr<Node>>& nodes);

void tridag(std::vector<std::shared_ptr<Node>>& nodes)
{
	int j;
	double bet;

    int n=static_cast<int>(nodes.size());
    std::vector<double> gam(n);
	if (nodes[0]->d2==0.0) 
      std::cerr << "Error 1 in tridag";
	
	nodes[0]->vNew=nodes[0]->r/(bet=nodes[0]->d2);
	for(j=1;j<n;j++) {
	  gam[j]=nodes[j-1]->d3/bet;
	  bet=nodes[j]->d2-nodes[j]->d1*gam[j];
	  if(bet == 0.0) 
        std::cerr << "Error 2 in tridag";
	  nodes[j]->vNew=(nodes[j]->r-nodes[j]->d1*nodes[j-1]->vNew)/bet;
	}
	for(j=(n-2);j>=0;j--){
	  nodes[j]->vNew -= gam[j+1]*nodes[j+1]->vNew;
	}
}
} //LongQt

#endif
