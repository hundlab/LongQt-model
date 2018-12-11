#ifndef CELL_KERNEL_HPP
#define CELL_KERNEL_HPP

#include<cellutils_core.h>

namespace LongQt {
//Macro to create a flag enum with a map of its values used by subclasses to create
//Options enum
#define EXPAND(x) x
// NUM_ARGS(...) evaluates to the literal number of the passed-in arguments.
#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _9, _10, N, ...) N
#define NUM_ARGS(...) EXPAND(_GET_NTH_ARG(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0))

#define CLASS_FUNCTIONS \
virtual std::map<std::string,int> optionsMap() const;\
virtual int option() const;\
virtual std::string optionStr() const;\
virtual void setOption(std::string opt);\
virtual void setOption(int opt);

#define REVERSE_1(a) a
#define REVERSE_2(a,b) b,a
#define REVERSE_3(a,...) EXPAND(REVERSE_2(__VA_ARGS__)),a
#define REVERSE_4(a,...) EXPAND(REVERSE_3(__VA_ARGS__)),a
#define REVERSE_5(a,...) EXPAND(REVERSE_4(__VA_ARGS__)),a
#define REVERSE_6(a,...) EXPAND(REVERSE_5(__VA_ARGS__)),a
#define REVERSE_7(a,...) EXPAND(REVERSE_6(__VA_ARGS__)),a
#define REVERSE_8(a,...) EXPAND(REVERSE_7(__VA_ARGS__)),a
#define REVERSE_9(a,...) EXPAND(REVERSE_8(__VA_ARGS__)),a
#define REVERSE_10(a,...) EXPAND(REVERSE_9(__VA_ARGS__)),a
#define REVERSE1(N,...) EXPAND(REVERSE_ ## N(__VA_ARGS__))
#define REVERSE(N, ...) REVERSE1(N, __VA_ARGS__)


#define EN_STR_0() WT = 0
#define EN_STR_1(str) str = 1 << 0, EN_STR_0()
#define EN_STR_2(str,...) str = 1 << 1, EXPAND(EN_STR_1(__VA_ARGS__))
#define EN_STR_3(str,...) str = 1 << 2, EXPAND(EN_STR_2(__VA_ARGS__))
#define EN_STR_4(str,...) str = 1 << 3, EXPAND(EN_STR_3(__VA_ARGS__))
#define EN_STR_5(str,...) str = 1 << 4, EXPAND(EN_STR_4(__VA_ARGS__))
#define EN_STR_6(str,...) str = 1 << 5, EXPAND(EN_STR_5(__VA_ARGS__))
#define EN_STR_7(str,...) str = 1 << 6, EXPAND(EN_STR_6(__VA_ARGS__))
#define EN_STR_8(str,...) str = 1 << 7, EXPAND(EN_STR_7(__VA_ARGS__))
#define EN_STR_9(str,...) str = 1 << 8, EXPAND(EN_STR_8(__VA_ARGS__))
#define EN_STR_10(str,...) str = 1 << 9, EXPAND(EN_STR_9(__VA_ARGS__))

#define MP_STR_0() {"WT",0}
#define MP_STR_1(str) {#str, 1 << 0}, MP_STR_0()
#define MP_STR_2(str,...) {#str, 1 << 1}, EXPAND(MP_STR_1(__VA_ARGS__))
#define MP_STR_3(str,...) {#str, 1 << 2}, EXPAND(MP_STR_2(__VA_ARGS__))
#define MP_STR_4(str,...) {#str, 1 << 3}, EXPAND(MP_STR_3(__VA_ARGS__))
#define MP_STR_5(str,...) {#str, 1 << 4}, EXPAND(MP_STR_4(__VA_ARGS__))
#define MP_STR_6(str,...) {#str, 1 << 5}, EXPAND(MP_STR_5(__VA_ARGS__))
#define MP_STR_7(str,...) {#str, 1 << 6}, EXPAND(MP_STR_6(__VA_ARGS__))
#define MP_STR_8(str,...) {#str, 1 << 7}, EXPAND(MP_STR_7(__VA_ARGS__))
#define MP_STR_9(str,...) {#str, 1 << 8}, EXPAND(MP_STR_8(__VA_ARGS__))
#define MP_STR_10(str,...) {#str, 1 << 9}, EXPAND(MP_STR_9(__VA_ARGS__))


#define _MAKE_MAP1(N, ...) std::map<std::string,int> optsMap = {EXPAND(MP_STR_ ## N(__VA_ARGS__))};
#define _MAKE_MAP(N, ...) _MAKE_MAP1(N, __VA_ARGS__)

#define _MAKE_ENUM1(N, ...) enum Options {EXPAND(EN_STR_ ## N(__VA_ARGS__))}; CLASS_FUNCTIONS
#define _MAKE_ENUM(N, ...) _MAKE_ENUM1(N, __VA_ARGS__)

#define _MAKE_OPTIONS1(N, ...) _MAKE_ENUM(N,__VA_ARGS__) _MAKE_MAP(N,__VA_ARGS__)
#define _MAKE_OPTIONS(N, ...) _MAKE_OPTIONS1(N,REVERSE(N,__VA_ARGS__))
//The function to call will set up enum Options
#define MAKE_OPTIONS(...) _MAKE_OPTIONS(NUM_ARGS(__VA_ARGS__),__VA_ARGS__)


#define OPERATORS(CLASS_NAME) \
inline CLASS_NAME::Options operator~ (CLASS_NAME::Options a) { return (CLASS_NAME::Options)~(int)a; }\
inline CLASS_NAME::Options operator| (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a | (int)b); }\
inline CLASS_NAME::Options operator& (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a & (int)b); }\
inline CLASS_NAME::Options operator^ (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a ^ (int)b); }\
inline CLASS_NAME::Options& operator|= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a |= (int)b); }\
inline CLASS_NAME::Options& operator&= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a &= (int)b); }\
inline CLASS_NAME::Options& operator^= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a ^= (int)b); }

#define OPTIONS_FUNCTIONS(CLASS_NAME) \
std::map<std::string, int> CLASS_NAME::optionsMap() const\
{\
    return optsMap;\
}\
int CLASS_NAME::option() const\
{\
    return opts;\
}\
std::string CLASS_NAME::optionStr() const\
{\
    std::string str = "";\
    bool first = true;\
    for (auto& it: optsMap) {\
        if (it.second & this->opts) {\
            if(first) {\
                str = it.first;\
                first = false;\
            } else {\
                str += "|"+it.first;\
            }\
        }\
    }\
    return str != "" ? str: "WT";\
}\
void CLASS_NAME::setOption(std::string opt)\
{\
    Options o = WT;\
    auto splits = CellUtils::split(opt,'|');\
    for(auto& sp: splits) {\
        try {\
            o |= static_cast<Options>(this->optsMap.at(sp));\
        } catch(std::out_of_range&) {\
            Logger::getInstance()->write<std::out_of_range>("CellKernel: Cell Option {} does not exist", sp);\
        }\
    }\
    this->setOption(o);\
}\
void CLASS_NAME::setOption(int opt)\
{\
    opts = static_cast<Options>(this->removeConflicts(opt));\
}

#define MAKE_OPTIONS_FUNCTIONS(CLASS_NAME) OPERATORS(CLASS_NAME) OPTIONS_FUNCTIONS(CLASS_NAME)

} //LongQt
#endif // CELL_KERNEL_HPP
