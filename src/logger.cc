#include "include/logger.h"
#include <iostream> 
using namespace std;

namespace Logger {

    void log_info(string message, bool parallel) {
        if (parallel) {
            cout << "INFO (Parallel) - " << message << "\n";
        } else {
            cout << "INFO (Sequential) - " << message << "\n";
        }
        
    }

    void log_error(string error, bool parallel) {
        if (parallel) {
            cout << "ERROR (Parallel) - " << error << "\n";
        } else {
            cout << "ERROR (Sequential) - " << error << "\n";
        }
        
    }
    
}