/**
 * @file
 * a example source file for browsing services and publishing and updating a service with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/ServiceDiscovery.hpp>
 
using namespace servicediscovery::avahi;
 
void testUpdated (RemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: UPDATED SERVICE: " << rms.getName() << std::endl;
}

void testAdded (ServiceEvent e) {
        ServiceConfiguration configuration = e.getServiceConfiguration();
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << configuration.getName() << " type: " << configuration.getType() << std::endl;
}

 
int main(int argc, char** argv)
{
	//create a service browser
        std::vector<std::string> serviceTypes;
        serviceTypes.push_back("_sd_test._tcp");
	ServiceDiscovery serviceListener;
        serviceListener.listenOn(serviceTypes);

	//connect a callback to the service added signal. Method can also be a class member. Look at sigc++ api
	serviceListener.addedComponentConnect(sigc::ptr_fun(testAdded));
	
	//publish a sample service to test the callbacks
	ServiceConfiguration conf("MyTestService", "_sd_test._tcp");
        conf.setDescription("service_year","1999");
        ServiceDiscovery servicePublisher;
        servicePublisher.start(conf);

        std::cout << "Sleep for 20s -- waiting for published service" << std::endl;
	sleep(20);
	
	//update the signal txt record
        conf.setDescription("somethingelse","10");
	std::cout << "Updating string list\n";
	servicePublisher.update(conf);
	
	sleep(5);
	
	//print services
	std::cout << "PRINTING FOUND SERVICESin _sd_test._tcp:\n";
        SearchPattern pattern(".*",".*",".*",".*");

        std::vector<ServiceDescription> services =  servicePublisher.findServices(pattern);
        std::vector<ServiceDescription>::iterator it;
	for (it = services.begin() ; it != services.end() ; it++) 
        {
                ServiceDescription description = *it;
		std::cout << "     SERVICE: " << description.getName() << " " << description.getType() << " " << description.getInterfaceIndex() << std::endl;
	}
	std::cout << "FINISHED PRINTING SERVICES.\n";

	sleep(5);
}
