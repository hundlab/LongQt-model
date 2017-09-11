#include "cellpvars.h"

CellPvars::~CellPvars() {
//    for(auto pvar : pvars) {
//        delete pvar.second;
//    }
}

void CellPvars::erase(string elem) {
    this->pvars.erase(elem);
}
CellPvars::IonChanParam* CellPvars::at(string elem) {
    return this->pvars.at(elem);
}
CellPvars::const_iterator CellPvars::begin() const {
    return this->pvars.begin();
}
CellPvars::const_iterator CellPvars::end() const {
    return this->pvars.end();
}
int CellPvars::size() const {
    return this->pvars.size();
}
void CellPvars::clear() {
    this->pvars.clear();
}
string CellPvars::IonChanParam::str(string name) {
    string info = name;
    string var1Text;
    string var2Text;
    switch(this->dist) {
        case Distribution::none:
            var1Text = "\tIncrementing\tInitial Value: ";
            var2Text = "\tIncrement Amount: ";
            break;
        case Distribution::normal:
            var1Text = "\tNormal Distribution\tMean: ";
            var2Text = "\tStandard Deviation: ";
            break;
        case Distribution::lognormal:
            var1Text = "\tLognormal Distribution\tMean: ";
            var2Text = "\tStandard Deviation: ";
            break;
    }
    info += var1Text+std::to_string(this->val[0])+var2Text+std::to_string(this->val[1]);
    return info;
}
