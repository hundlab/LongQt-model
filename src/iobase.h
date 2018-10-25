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
namespace LongQt {

class IOBase {
    public:
    protected:
        virtual bool write(std::set<std::string> selection, std::map<std::string, double*> map, std::ofstream* ofile);
        virtual bool setOutputfile(std::string filename, std::set<std::string> set, std::ofstream* ofile);
    virtual void closeFile(std::ofstream* ofile);
};

} //LongQt

#endif
