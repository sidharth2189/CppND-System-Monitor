#include <string>
#include <vector>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

long Processor::user_{0};
long Processor::nice_{0};
long Processor::system_{0};
long Processor::idle_{0};
long Processor::iowait_{0};
long Processor::irq_{0};
long Processor::softirq_{0};
long Processor::steal_{0};
long Processor::guest_{0};
long Processor::guest_nice_{0};
long Processor::prevuser_{0};
long Processor::prevnice_{0};
long Processor::prevsystem_{0};
long Processor::previdle_{0};
long Processor::previowait_{0};
long Processor::previrq_{0};
long Processor::prevsoftirq_{0};
long Processor::prevsteal_{0};
long Processor::prevguest_{0};
long Processor::prevguest_nice_{0};

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    float utilization = 0.0;
    
    // Read from current info from Linux parser
    vector<string> cpuTimes = LinuxParser::CpuUtilization();
    user_ = stol(cpuTimes[LinuxParser::CPUStates::kUser_]);
    nice_ = stol(cpuTimes[LinuxParser::CPUStates::kNice_]);
    system_ = stol(cpuTimes[LinuxParser::CPUStates::kSystem_]);
    idle_ = stol(cpuTimes[LinuxParser::CPUStates::kIdle_]);
    iowait_ = stol(cpuTimes[LinuxParser::CPUStates::kIOwait_]);
    irq_ = stol(cpuTimes[LinuxParser::CPUStates::kIRQ_]);
    softirq_ = stol(cpuTimes[LinuxParser::CPUStates::kSoftIRQ_]);
    steal_ = stol(cpuTimes[LinuxParser::CPUStates::kSteal_]);
    guest_ = stol(cpuTimes[LinuxParser::CPUStates::kGuest_]);
    guest_nice_ = stol(cpuTimes[LinuxParser::CPUStates::kGuestNice_]);

    // Calculate utilization
    long prevIdle =  previdle_ + previowait_;
    long idle = idle_ + iowait_;

    long prevNonIdle = prevuser_ + prevnice_ + prevsystem_ + \
    previrq_ + prevsoftirq_ + prevsteal_;
    long nonIdle = user_ + nice_ + system_ + irq_ + softirq_ + steal_;

    long prevTotal = prevIdle + prevNonIdle;
    long total = idle + nonIdle;

    long totaled = total - prevTotal;
    long idled = idle - prevIdle;
    
    if (totaled != 0) { utilization = ((totaled - idled)*1.0)/(totaled*1.0); }

    // Update previous info with current info
    prevuser_ = user_;
    prevnice_ = nice_;
    prevsystem_ = system_;
    previdle_ = idle_;
    previowait_ = iowait_;
    previrq_ = irq_;
    prevsoftirq_ = softirq_;
    prevsteal_ = steal_;
    prevguest_ = guest_;
    prevguest_nice_ = guest_nice_;
    
    // Return utilization
    return utilization;
}