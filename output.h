//################################################
// This header file
// declares Output class used to read and write
// values of variables supplied as argument (varmap).
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef CELLOUTPUT
#define CELLOUTPUT

#include <map>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Output
{
public:
    Output();
    Output(const Output& toCopy);
    ~Output();
    
    ofstream ofile;
    int counter;
    int interval;   //write to file when counter%interval==0
    
    virtual int writevals(map<string, double*> varmap, string file, char type);
    virtual int writevals(map<string, double*> varmap, string file);
    virtual int writevals(map<string, double> varmap, string file, char type);
    virtual int writevals(map<string, double> varmap, string file);
};

#endif
