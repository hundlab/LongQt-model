#include <cstdlib>
#include <cstdio>
#include <vector>

#ifndef TRIDAG_H
#define TRIDAG_H

void tridag(vector<double> &a, vector<double> &b, vector<double> &c, vector<double> &r, vector<double> &u);

void tridag(vector<double> &a, vector<double> &b, vector<double> &c, vector<double> &r, vector<double> &u)
{
	int j;
	double bet;

	int n=a.size();
	vector<double> gam(n);
	if (b[0]==0.0) 
	  cerr << "Error 1 in tridag";
	
	u[0]=r[0]/(bet=b[0]);
	for(j=1;j<n;j++) {
	  gam[j]=c[j-1]/bet;
	  bet=b[j]-a[j]*gam[j];
	  if(bet == 0.0) 
	    cerr << "Error 2 in tridag";
	  u[j]=(r[j]-a[j]*u[j-1])/bet;
	}
	for(j=(n-2);j>=0;j--){
	  u[j] -= gam[j+1]*u[j+1];
	}
}

#endif
