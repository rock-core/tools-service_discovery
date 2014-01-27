#include "ServicePattern.hpp"

#include <sstream>
#include <cmath>
#include <iostream>

namespace servicediscovery {
namespace avahi {

bool SearchPattern::isMatching(const ServiceDescription& description) const
{
    if(!regex_match(description.getName(), namePattern))
    {
        return false;
    }
      
    // Check labels and descriptions          
    std::vector<std::string> labels = description.getLabels();
    if(labels.empty())
    {
        if(!regex_match("", labelPattern))
        {
    	    return false;
        }
    } else {
          std::vector<std::string>::const_iterator it = labels.begin(); 
          bool match = false;
          for(; it != labels.end(); ++it)
          {
              std::string label = *it;
              if(regex_match(label, labelPattern))
              {
                  // okay, we have a least one match now check description field
                  std::string txt = description.getDescription(label);
                  if(regex_match(txt, txtPattern))
                  {
                      match = true;
                      break;
                  }
              }
          }
          if(!match)
          {
              return false;
          }
    }
      
    if(!regex_match(description.getType(), typePattern))
    {
        return false;
    }
      
    return true;
}

namespace pattern
{
std::string castFlags(pattern::Flags flags)
{
    std::stringstream oss;
    oss << flags;
    return oss.str();
}
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
    if(label == "*") 
    {
        std::vector<std::string>::iterator it;
        std::vector<std::string> labellist = service.getLabels();

        for(it = labellist.begin(); it != labellist.end(); it++) 
        {
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
    {
        return false;
    }

    int serviceflags = atoi(desc.c_str());

    if(serviceflags == 0 && desc != "0")
    {
        return false;
    }

    return ((serviceflags & flags) == flags);
}

// ---------------------------------------------------------------------------- 

bool MultiPattern::matchDescription(const ServiceDescription& service) const
{
    std::vector<const ServicePattern*>::const_iterator it;
  
    for(it = patterns.begin(); it != patterns.end(); it++)
    {
        if( (*it)->matchDescription(service) == false )
        {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------- 

bool AuthorityPattern::matchDescription(const ServiceDescription& service) const
{
    std::string desc = service.getDescription("remote");

    if(desc.empty())
    {
        return false;
    }

    // last colon is a separator for authority rate to the other remote attributes
    size_t position = desc.find_last_of(":");
    std::string authStr = desc.substr(position);

    // position + 1 => erase colon from substring
    int a = atoi(desc.substr(position + 1).c_str());

    if(a == 0 && authStr != "0")
    {
        return false;
    }

    return atleast ? authority <= a : authority >= a;
}

} // end namespace avahi
} // end namespace servicediscovery
