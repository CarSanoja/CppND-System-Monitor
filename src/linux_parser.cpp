#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "format.h"
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/*

Linux stores data about individual processes in files within subdirectories 
of the /proc directory. Each subdirectory is named for that particular 
process's identifier number. The data that this project requires exists in 
those files.

This library helps to obtain every information saved in those directories

*/


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
/*
htop author here. These are the calculations I make to get the numbers for the green, blue and yellow bars in the memory meter:

    Total used memory = MemTotal - MemFree

    Dividing for the entiry memory we obtain thepercentage of usage
*/
float LinuxParser::MemoryUtilization()
{
  float total_memory;
  float free_memory;
  string key;
  string value;
  string line;
  string extra;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
  		std::istringstream linestream(line);
    	while ( linestream >> key >> value >> extra) {
        	if (key == "MemTotal") 
          { 
            total_memory = std::stof(value);
          }
          if (key == "MemFree") 
          { 
            free_memory = std::stof(value);
          }
        	}
    	}
  }
  return (total_memory - free_memory) / total_memory;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime()
{
  long int _uptime;
  string uptime; 
  string i_time;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  	std::istringstream linestream(line);
    linestream >> uptime >> i_time;
    _uptime = std::stol(uptime);
    }
  return _uptime;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function

/*

  To calculate CPU usage for a specific process you'll need the following:

    /proc/uptime
        #1 uptime of the system (seconds)
    /proc/[PID]/stat
        #14 utime - CPU time spent in user code, measured in clock ticks
        #15 stime - CPU time spent in kernel code, measured in clock ticks
        #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
        #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
        #22 starttime - Time when the process started, measured in clock ticks
    Hertz (number of clock ticks per second) of your system.
        In most cases, getconf CLK_TCK can be used to return the number of clock ticks.
        The sysconf(_SC_CLK_TCK) C function call may also be used to return the hertz value.


*/
long LinuxParser::ActiveJiffies(int pid) 
{
  string value;
  string line;
  long t_clk;
  vector<string> aux;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
			aux.push_back(value);
        }
    }
  long int u_time = std::stol(aux[13]);
  long int s_time = std::stol(aux[14]);
  long int cu_time = std::stol(aux[15]);
  long int cs_time = std::stol(aux[16]);
  t_clk = (u_time + s_time + cu_time + cs_time);
  return t_clk;
}


// TODO: Read and return CPU utilization

vector<string> LinuxParser::CpuUtilization()
{
  string value;
  string key;
  string line;
  vector<string> jiffies_list;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  	std::istringstream linestream(line);
   	while (linestream >> key){
    	if (key != "cpu"){
          jiffies_list.push_back(key);}
    }
    }
  return jiffies_list;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  int val;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line))
    {
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) 
      {
        if (key == "processes") 
        { 
          val = std::stoi(value);
          return val;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  int val;
  string key;
  string value; 
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line))
    {
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) 
      {
        if (key == "procs_running") 
        { 
          val = std::stoi(value);
          return val;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{
  string obj;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line))
    {
  		std::istringstream linestream(line);
      std::replace(line.begin(), line.end(), ' ', '*');
    	while (linestream >> obj) 
      {
			  std::replace(obj.begin(), obj.end(), '*', ' ');
        return obj;
      }
    }
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid)
{
  string key, value, kb, line;
  int vmsize;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> kb) {
        	if (key == "VmSize:") { 
              	vmsize = std::stoi(value); 
              	vmsize = vmsize/1000;
              	return std::to_string(vmsize);
               }
        	}
      	}
    }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function

/*

Each process has an associated user identifier (UID), corresponding to the 
process owner. This means that determining the process owner requires two 
steps:

    Find the UID associated with the process
    Find the user corresponding to that UID

The UID for a process is stored in /proc/[PID]/status.

For the purposes of this project, simply need to capture the first integer on the "Uid:" line.

*/
string LinuxParser::Uid(int pid) 
{
  string key;
  string value; 
  string uid; 
  string user; 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line))
    {
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) 
      {
        if (key == "Uid:") 
        { 
          uid = value;
        }
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid)
{
  string user; 
  string direc; 
  string u_id; 
  string line;
  string uid_ = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
    	while (linestream >> user >> direc >> u_id) 
      {
        if (u_id == uid_)
        {
          return user;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{
  string value;
  string line;
  long int start; 
  long int up_time;
  vector<string> aux;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) 
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) 
    {
			aux.push_back(value);
    }
  }
  /*
  Note that the "starttime" value in this file is measured in "clock ticks". In order to convert from "clock ticks" to seconds, you must:

    #include <unistd.h>
    divide the "clock ticks" value by sysconf(_SC_CLK_TCK)

  Once you have converted the time value to seconds, you can use the Format::Time() function from the project starter code to display the seconds in a "HH:MM:SS" format.
  */
 /*
 std::stol(): This function converts the string, provided as an argument in the function call, to long int. It parses str interpreting its content as an integral number 
 of the specified base, which is returned as a value of type long int. 
 */
  start = std::stol(aux[21])/sysconf(_SC_CLK_TCK);
  //std::cout << "usage " << start << std::endl;
  up_time =  LinuxParser::UpTime() - start;
  //std::cout << "usage " << up_time << std::endl;
  string up_time_format = Format::ElapsedTime(up_time);
  //std::cout << "up time " << up_time_format<< std::endl;
  return up_time;
}