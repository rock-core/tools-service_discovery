/**
 * @file
 * a example source file for browsing services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace servicediscovery::avahi;

void callbackFunction (ServiceEvent rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getServiceConfiguration().getName() << std::endl;
}

 
int main(int argc, char** argv)
{

	std::string someServiceData = "IOR:010000001e00000049444c3a5254542f436f7262612f436f6e74726f6c5462736b3a312e30000000010000000000000068000000010102000f0000003139322e3136382e3130312e3638000026e300000e000000fe35313c4c0000278c000000000000000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100";
	ServiceDiscovery service;
	ServiceConfiguration conf("ModuleB", "_module._tcp");
	conf.setDescription("IOR", someServiceData);
	service.addedComponentConnect(sigc::ptr_fun(callbackFunction));
	service.start(conf);
	sleep(150);

	return 0;
}
