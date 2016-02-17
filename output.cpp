//################################################
// This code file
// defines Output class used to read and write
// values of variables supplied as argument (varmap).
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################


#include "output.h"

//#############################################################
// Output class constructor and destructor
//#############################################################

Output::Output()
{
    counter = 0;
    interval = 1;
    
};

Output::Output(const Output& toCopy) {
    counter = 0; //= toCopy.counter;
    interval = 1; //= toCopy.interval;
}

Output::~Output()
{
    if(ofile.is_open()){
        ofile.close();
        ofile.clear();
    }
};

//#############################################################
// Write values of variables in varmap to file. Output is
// organized with variable name in 1st column and values
// in 2nd column.
//#############################################################
int Output::writevals(map<string, double*> varmap, string file)
{
    map<string, double*>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
        exit(1);
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->first << "\t" << *p->second << "\n";
    
    ofile.close();
    return 1;
};

//#############################################################
// Write values of variables in varmap to file more than once.
// Output is organized in columns with variable names in first
// row.
//#############################################################
int Output::writevals(map<string, double*> varmap, string file, char type)
{
    map<string, double*>::iterator p;
    ofile.precision(10);
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            cout << "Error opening " << file << endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p)
            ofile << p->first << "\t";
        ofile << endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p) {
        ofile << *p->second << "\t";
    }
    
    ofile << endl;
    
    counter++;
    return 1;
};

//#############################################################
// Write values of variables in varmap to file. Output is
// organized with variable name in 1st column and values
// in 2nd column.
//#############################################################
int Output::writevals(map<string, double> varmap, string file)
{
    map<string, double>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
        exit(1);
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->first << "\t" << p->second << "\n";
    
    ofile.close();
    return 1;
};

//#############################################################
// Write values of variables in varmap to file more than once.
// Output is organized in columns with variable names in first
// row.
//#############################################################
int Output::writevals(map<string, double> varmap, string file, char type)
{
    map<string, double>::iterator p;
    ofile.precision(10);
    
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            cout << "Error opening " << file << endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p) {
            ofile << p->first << "\t";
        }
        ofile << endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p) {
        ofile << p->second << "\t";
    }
    
    ofile << endl;
    counter++;
    return 1;
};


