#ifndef RIMRES_SERVICEPATTERN_CORE_H_
#define RIMRES_SERVICEPATTERN_CORE_H_

#include <string>
#include <vector>
#include "ServiceConfiguration.h"

namespace dfki { namespace communication {

namespace pattern {
  const int BUSY = 1;
  const int READY = 2;

  std::string castFlags(int flags);  
}

/**
 * Abstract base class for ServicePattern. Provides a simple interface for
 * searching via specific service patterns (position, type, group, state, etc.)
 */
struct ServicePattern {
  virtual bool matchDescription(const ServiceDescription& service) const = 0;
};

// ----------------------------------------------------------------------------

struct PropertyPattern : public ServicePattern {
  PropertyPattern() : label("*"), description("*") {}
  PropertyPattern(std::string label, std::string description)
   : label(label), description(description) {}
  ~PropertyPattern() {}

  bool matchDescription(const ServiceDescription& service) const;

 private:
  std::string label;
  std::string description;
};

// ----------------------------------------------------------------------------

struct PositionPattern : public ServicePattern {
  PositionPattern(int x, int y, int z, double distance) 
    : x(x), y(y), z(z), distance(distance) {}
  ~PositionPattern() {}

  bool matchDescription(const ServiceDescription& service) const;

 private:
  int x, y, z;    
  double distance;
};

// ----------------------------------------------------------------------------

struct FlagPattern : public ServicePattern {
  FlagPattern(int flags) : flags(flags) {}
  ~FlagPattern() {}

  bool matchDescription(const ServiceDescription& service) const;

 private:
  int flags;
};

// ----------------------------------------------------------------------------

struct MultiPattern : public ServicePattern {
  MultiPattern() {}
  ~MultiPattern() {}

  bool matchDescription(const ServiceDescription& service) const;

  MultiPattern& operator<<(const ServicePattern& pattern) {
    patterns.push_back(&pattern);
    return *this;
  }

  private:
    std::vector<const ServicePattern*> patterns;
};

}}


#endif // RIMRES_SERVICEPATTERN_CORE_H_
