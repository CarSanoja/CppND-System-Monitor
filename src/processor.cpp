#include "processor.h"

// TODO: Return the aggregate CPU utilization

/*

Trying to follow the stack overflow algorithm mentioned in class:

According the htop source code at the time of writing, my assumptions looks like they are valid:

(see void ProcessList_scan(ProcessList* this) function at ProcessList.c)

// Guest time is already accounted in usertime
usertime = usertime - guest;                     # As you see here, it subtracts guest from user time
nicetime = nicetime - guestnice;                 # and guest_nice from nice time
// Fields existing on kernels >= 2.6
// (and RHEL's patched kernel 2.4...)
idlealltime = idletime + ioWait;                 # ioWait is added in the idleTime
systemalltime = systemtime + irq + softIrq;
virtalltime = guest + guestnice;
totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

Algorithmically, we can calculate the CPU usage percentage like:

PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
NonIdle = user + nice + system + irq + softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald


*/
float Processor::Utilization()
{
    // obtain all jiffies from our helper
    std::vector<std::string> jiffies_list = LinuxParser::CpuUtilization();

    for (size_t i = 0; i < jiffies_list.size(); i++ ){
        if ( i != LinuxParser::kGuest_ and i != LinuxParser::kGuestNice_ ){
            total_jiffies += std::stof(jiffies_list[i]);
        }
        if (i==LinuxParser::kIdle_ or i==LinuxParser::kIOwait_){
            ti_jiffies += std::stof(jiffies_list[i]);
        }
    }
    usage = (total_jiffies - ti_jiffies) / total_jiffies;
    //std::cout << "usage " << usage << std::endl;
    return usage; 
}