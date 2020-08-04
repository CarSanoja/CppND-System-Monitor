#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
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
    
    return usage; 
}