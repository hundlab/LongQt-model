//################################################
// This header file
// declares the abstract class used 
// as a base class to read and write
// values of variables supplied as argument (varmap).
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef IOBASE_H
#define IOBASE_H

#include <map>
#include <string>
#include <set>
#include <iostream>
#include <fstream>

using namespace std;

class IOBase {
    public:
    protected:
        virtual bool write(set<string> selection, map<string, double*> map, ofstream* ofile);
        virtual bool setOutputfile(string filename, set<string> set, ofstream* ofile);
	virtual void closeFile(ofstream* ofile);
};


#endif
