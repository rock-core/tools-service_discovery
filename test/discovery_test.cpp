#include <boost/test/unit_test.hpp>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace std;
using namespace servicediscovery::avahi;

struct ServiceLandscape
{
  ServiceLandscape() : domain("_sd_test._tcp") 
  {
    srand(time(NULL));
    ServiceConfiguration com1 = createConfiguration("Com1");
    ServiceConfiguration com2 = createConfiguration("Com2");
    ServiceConfiguration com3 = createConfiguration("Com3");
    ServiceConfiguration power1 = createConfiguration("Power1");
    ServiceConfiguration find = createConfiguration("FindService");
    
    com1.setDescription("location", "1,0,0");
    com2.setDescription("location", "0,1,0");
    com3.setDescription("location", "0,0,1");
    power1.setDescription("location", "2,0,1");

    com1.setDescription("service", "1,-1,0:communication");
    com2.setDescription("service", "2,-1,0:lunar_mission::robot:communication");
    com3.setDescription("service", "3,-1,0:lunar_mission::communication");
    power1.setDescription("service", "4,1,0:power");

    com1.setDescription("remote", "none:50");
    com2.setDescription("remote", "none:50");
    com3.setDescription("remote", "none:75");
    power1.setDescription("remote", "none:100");

    com1.setDescription("flags", pattern::castFlags(pattern::BUSY));
    com2.setDescription("flags", pattern::castFlags(pattern::READY));
    com3.setDescription("flags", pattern::castFlags(pattern::READY));
    power1.setDescription("flags", pattern::castFlags(pattern::BUSY));

    BOOST_TEST_MESSAGE("Starting com1");
    startService(com1);

    BOOST_TEST_MESSAGE("Starting com2");
    startService(com2);

    BOOST_TEST_MESSAGE("Starting com3");
    startService(com3);

    BOOST_TEST_MESSAGE("Starting power");
    startService(power1);

    watcher = new ServiceDiscovery();
    std::vector<std::string> types;
    types.push_back(domain);
    watcher->listenOn(types);

    BOOST_TEST_MESSAGE("Waiting 10s for finishing the initialization process for all services");
    sleep(20);
  }

  ~ServiceLandscape() {
    vector<ServiceDiscovery*>::iterator it;
    for(it = ServiceInfos.begin(); it != ServiceInfos.end(); it++) {
      // automatically done via ServiceDiscovery destructor
      delete *it;
    }
  }

 private:
  ServiceConfiguration createConfiguration(std::string name) const {
    ostringstream sin;
    sin << createRandomIOR();
    
    ServiceConfiguration sc(name, domain);
    sc.setDescription("IOR", sin.str());
    return sc;
  }

  int createRandomIOR() const {
    int number = 0;
    
    for(int i = 0; i < 3; i++) {
      number |= (0xFFFF & (rand() % 0xFFFF));
    }

    return number;
  }

  ServiceDiscovery* startService(const ServiceConfiguration& conf) {
    ServiceDiscovery* sc = new ServiceDiscovery;
    ServiceInfos.push_back(sc);
    sc->start(conf);

    return sc;
  }

 protected:
  const char* domain;
  
  ServiceDiscovery* watcher;

 private:
  vector<ServiceDiscovery*> ServiceInfos;
};

BOOST_FIXTURE_TEST_SUITE( ServiceDiscoverySuite, ServiceLandscape )

BOOST_AUTO_TEST_CASE( findServices )
{
  vector<ServiceDescription> result;

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES
  // --------------------------------------------------------------------------

  // PropertyPattern() uses label = "*", description = "*"
  result = watcher->findServices(PropertyPattern());      

  BOOST_REQUIRE(4 /*utility services */ + 1 /*the watch service itself*/ <= result.size());  

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES MATCHING A PROPERTY
  // --------------------------------------------------------------------------

  // Find a specific property 
  result = watcher->findServices(PropertyPattern("service", ".*com.*")); 

  sleep(10);

  BOOST_REQUIRE_MESSAGE(3 <= result.size(), "Result size: " << result.size());

  for(size_t i = 0; i < result.size(); i++)
  {
    if(result[i].getName() == "Com1")
    {
        BOOST_CHECK_EQUAL("1,-1,0:communication", result[i].getDescription("service") );
    }
  }

  result = watcher->findServices(PropertyPattern("location", "0,0,1"));
  
  BOOST_REQUIRE(1 <= result.size());
  
  BOOST_CHECK_EQUAL("Com3", result[0].getName());

  // Find a specific description/label via wildcards
  result = watcher->findServices(PropertyPattern("*", "communication"));

  BOOST_REQUIRE(3 <= result.size());

  result = watcher->findServices(PropertyPattern("location"));

  BOOST_REQUIRE(4 <= result.size());

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES MATCHING A FLAG
  // --------------------------------------------------------------------------

  result = watcher->findServices(FlagPattern(pattern::READY));

  BOOST_REQUIRE(2 <= result.size());

  for(size_t i = 0; i < result.size(); ++i) {
    std::string name = result[i].getName();
    BOOST_CHECK(name == "Com2" || name == "Com3");
  }

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES MATCHING AN AUTHORITY
  // --------------------------------------------------------------------------
    
  result = watcher->findServices(AuthorityPattern(100));
  
  BOOST_REQUIRE(1 <= result.size());
  BOOST_CHECK(result[0].getName() == "Power1");

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES WITHIN A SPECIFIC NAMESPACE
  // --------------------------------------------------------------------------

  result = watcher->findServices(PropertyPattern(), "lunar_mission");

  BOOST_REQUIRE(2 <= result.size());

  result = watcher->findServices(PropertyPattern(), "lunar_mission::robot");
  
  BOOST_REQUIRE(1 <= result.size());
  
  BOOST_CHECK(result[0].getName() == "Com2");

  // --------------------------------------------------------------------------
  // FIND ALL SERVICES MATCHING A MULTIPLE PATTERNS
  // --------------------------------------------------------------------------

  MultiPattern multi;
  PropertyPattern p("service", "communication");
  FlagPattern f(pattern::READY);
  multi << p << f;

  // TODO: USING multi << PropertyPattern(..) << FlagPattern(..)
  // will maybe occur a segmentation fault

  result = watcher->findServices(multi);

  BOOST_REQUIRE(2 <= result.size());

}

BOOST_AUTO_TEST_CASE(DynamicCreateDelete)
{
    ServiceConfiguration conf("test_discovery", "_sd_test._tcp");
    ServiceDiscovery* serviceDiscovery = new ServiceDiscovery();

    serviceDiscovery->start(conf);

    sleep(20);

    delete serviceDiscovery;



}

BOOST_AUTO_TEST_SUITE_END()

