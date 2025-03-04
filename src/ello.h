//
// Created by keanu on 6/7/2024.
//

#ifndef RAYLIBSTARTER_ELLO_H
#define RAYLIBSTARTER_ELLO_H


#include <string>

class ello
{
public:
    ello(bool testP, std::string elloStrP) : test(testP), elloStr(elloStrP) {};
private:
    bool test;
    std::string elloStr;
};


#endif //RAYLIBSTARTER_ELLO_H
