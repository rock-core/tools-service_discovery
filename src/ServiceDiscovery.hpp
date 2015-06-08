#ifndef SERVICE_DISCOVERY_HPP
#define SERVICE_DISCOVERY_HPP
/*! \mainpage Service Discovery
 * The service discovery library is a C++-wrapper around existing libraries to
 * facilitate publishing and discovering services in a distributed system.
 *
 * Currently it relies on Avahi (http://avahi.org)
 *
 * \section Example
 * A number of examples for usage of this library can be found in the
 * src/examples folder of the source package.
 *
 * A simple usage example:
 * \verbatim

  #include <service_discovery/ServiceDiscovery.hpp>

  using namespace std;
  using namespace servicediscovery::avahi;

  ...

  ServiceConfiguration configuration("test_component","_sd_test_component._tcp");
  configuration.setDescription("description", "This is some information published with with component");

  // Publish a single service
  ServiceDiscovery servicePubisher;
  servicePublisher.start(configuration);


  // Listen for services publish under _sd_test_component._tcp
  std::vector<std::string> serviceTypes;
  serviceTypes.push_back("_sd_test_component._tcp");
  ServiceDiscovery serviceListener;
  serviceListener.listenOn(serviceTypes);

  // Allow services to publish
  sleep(5);


  // Check for existing services (under _sd_test_component._tcp)
  std::vector<ServiceDescription> services =  serviceListener.findServices();
  std::cout << " - Found services: " << std::endl;
  std::vector<ServiceDescription>::iterator it = services.begin();
  for(;it != services.end(); ++it)
  {
      std::cout << it->getName() << std::endl;
  }

  serviceListener.stop();
  serviceDiscovery.stop();

  ...
 \endverbatim
 *
 */
#include <service_discovery/impl/avahi/ServiceDiscovery.hpp>
#endif // SERVICE_DISCOVERY_HPP
