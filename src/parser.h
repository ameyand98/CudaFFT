#ifndef _PARSER_H_
#include <unistd.h> 
#include <string>
#include <iostream>

using namespace std;

class Parser {

public:
    bool parse(int argc, char** argv); 
    int runs = 100;
    string infile = "";
    string outfile = "";
    int length = 0;
    int threads = 0;
};

























#endif