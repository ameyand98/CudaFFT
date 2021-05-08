#include "parser.h"
#include <bits/getopt_core.h>
#include <string>

bool Parser::parse(int argc, char **argv) {
    int input;

    while ((input = getopt(argc, argv, "i:o:r:n:t:d:w:x:")) != -1) {
        switch (input) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'r':
                runs = stoi(optarg);
                break;
            case 'n':
                length = stoi(optarg);
                break;
            case 't':
                threads = stoi(optarg);
                break;
            case 'd':
                dim = stoi(optarg);
                break;
            case 'w':
                out_data = stoi(optarg);
                break;
            case 'x':
                integer = stoi(optarg);
                break;
        }
    }
    return true;
}