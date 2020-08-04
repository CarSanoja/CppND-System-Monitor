#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{
  auto HH = seconds / 3600 ;
  auto H_aux = seconds % 3600;
  auto MM = H_aux / 60;
  auto SS = H_aux % 60;
  string HH_str = std::to_string(HH);
  string MM_str = std::to_string(MM);
  string SS_str = std::to_string(SS);
  string time = HH_str + ':' + MM_str + ':' + SS_str;
  return time;
}