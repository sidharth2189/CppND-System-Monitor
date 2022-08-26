#include <string>
#include <sstream>

#include "format.h"

using std::string;

// Helper function to fromat time
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    std::stringstream duration;
    string init_hour = "";
    string init_min = "";
    string init_sec = "";
    string colon = ":";
    int hours = seconds/3600;
    int mins = (seconds%3600)/60;
    int secs = (seconds%3600)%60;
    if (hours < 10) { init_hour = "0"; }
    if (mins < 10) { init_min = "0"; }
    if (secs < 10) { init_sec = "0"; }
    duration << init_hour << hours << colon+init_min << mins << colon+init_sec << secs;
    return duration.str();
}