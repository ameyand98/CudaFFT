#ifndef _PARSER_H_
#include <unistd.h> 
#include <string>
#include <iostream>

using namespace std;

class Parser {

public:
    bool parse(int argc, char** argv); 
    string infile = "";
    string outfile = "";
    int length = 0;
    int threads = 0;
    int dim = 1;
    int out_data = 0;
    int integer = 0;
    int runs = 1;
    string image = "";
};

























#endif