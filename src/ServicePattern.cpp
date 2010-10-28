#include "ServicePattern.h"

#include <sstream>
#include <cmath>
#include <iostream>

namespace dfki { namespace communication {
// ----------------------------------------------------------------------------

std::string pattern::castFlags(pattern::Flags flags)
{
  std::stringstream oss;
  oss << flags;
  return oss.str();
}

// ----------------------------------------------------------------------------

bool PositionPattern::matchDescription(const ServiceDescription& service) const
{
  std::string position = service.getDescription("location");

  if(position.empty())
    return false;

  // Parse position into coordinate buffer via STL
  int coordinate[3];

  size_t begin = 0, comma;
  std::string str;

  for(int i = 0; i < 3; i++)
  {
    comma = position.find(",", begin);

    if(comma == std::string::npos && i != 2)
      return false;

    if(i == 2)
      str = position.substr(begin);
    else
      str = position.substr(begin, comma - begin);

    begin = comma + 1;    

    coordinate[i] = atoi(str.c_str());

    if(coordinate[i] == 0 && str != "0")
      return false;
  }

  // calculate euclidean distance between two points
  int dx = coordinate[0] - this->x;
  int dy = coordinate[1] - this->y;
  int dz = coordinate[2] - this->z;
  
  return sqrt(dx * dx + dy * dy + dz * dz) <= this->distance;
}

// ----------------------------------------------------------------------------

bool PropertyPattern::matchDescription(const ServiceDescription& service) const
{
  if(label == "*") {
    std::vector<std::string>::iterator it;
    std::vector<std::string> labellist = service.getLabels();

    for(it = labellist.begin(); it != labellist.end(); it++) {
      std::string desc = service.getDescription(*it);

      if(regex_search(desc, expression))
        return true;
    }

    return false;
  }

  std::string desc = service.getDescription(label);

  return !desc.empty() && regex_search(desc, expression);
}

// ----------------------------------------------------------------------------

bool FlagPattern::matchDescription(const ServiceDescription& service) const
{
  std::string desc = service.getDescription("flags");
  
  if(desc.empty())    
    return false;

  int serviceflags = atoi(desc.c_str());

  if(serviceflags == 0 && desc != "0")
    return false;

  return ((serviceflags & flags) == flags);
}

// ---------------------------------------------------------------------------- 

bool MultiPattern::matchDescription(const ServiceDescription& service) const
{
  std::vector<const ServicePattern*>::const_iterator it;
  
  for(it = patterns.begin(); it != patterns.end(); it++) {
    if( (*it)->matchDescription(service) == false )
      return false;
  }

  return true;
}

}}
