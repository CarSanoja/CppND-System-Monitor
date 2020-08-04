#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) 
{
    // it is neccesary the constructor bc system process declaration adds the pid number
  _pid = pid;
  _cpu = LinuxParser::ActiveJiffies(pid);
}

// TODO: Return this process's ID
int Process::Pid() 
{
    return _pid;
    //std::cout << "usage " << _pid << std::endl;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() 
{
    long int Hz = sysconf(_SC_CLK_TCK);
    double seconds;
    seconds = LinuxParser::UpTime() - ((float)_cpu / Hz);
    return ((float)_cpu / Hz / seconds );
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return _cpu > a._cpu; }