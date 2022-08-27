#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {    
    float utilization = 0.0;
    
    // Read current info from Linux parser
    vector<string> statInfos = LinuxParser::CpuUtilization(Process::Pid());
    long utime = stol(statInfos[13]);       // clock ticks
    long stime = stol(statInfos[14]);       // clock ticks
    long cutime = stol(statInfos[15]);      // clock ticks
    long cstime = stol(statInfos[16]);      // clock ticks
    long starttime = stol(statInfos[21]);   // clock ticks
    long uptime = LinuxParser::UpTime();    // seconds

    // Calculate cpu utilization of process
    long total_time = utime + stime + cutime + cstime;             // clock ticks
    long elapsed_time = uptime - (starttime/sysconf(_SC_CLK_TCK)); // seconds
    if (elapsed_time != 0) { 
        utilization = ((total_time*1.0)/sysconf(_SC_CLK_TCK))/(elapsed_time*1.0);
    }
    return utilization;
}

// Return the command that generated this process
string Process::Command() { 
    string command = LinuxParser::Command(Process::Pid());
    // Limit command for uniformity in length of display
    return command.substr(0,40);
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return Process::CpuUtilization() > a.Process::CpuUtilization();
}