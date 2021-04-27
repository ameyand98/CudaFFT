#ifndef _LOGGER_H_

#include <string>
using namespace std;

namespace Logger {

    extern void log_info(string message, bool parallel);
    extern void log_error(string error, bool parallel);
}

#endif