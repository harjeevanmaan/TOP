#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    std::stringstream stream;
    int hour = seconds/3600;
    int minutes = seconds/60 - hour*60;
    seconds = seconds - minutes*60 - hour*3600;
    
    stream << std::setfill('0') << std::setw(2) << hour << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
    return stream.str();
}