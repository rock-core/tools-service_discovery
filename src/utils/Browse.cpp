/**
 * @file
 * a utils source file for browsing services with the service_discovery implementation
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace servicediscovery::avahi;

void addService(ServiceEvent rms)
{
	std::cout << " + ADDED SERVICE: " << rms.getServiceConfiguration().getName() << std::endl;
}

void removeService(ServiceEvent rms)
{
	std::cout << " - REMOVED SERVICE: " << rms.getServiceConfiguration().getName() << std::endl;
}
 
int main(int argc, char** argv)
{
    std::vector<std::string> domains;
    if(argc > 1)
    {
        for(int i = 1; i < argc; ++i)
        {
            domains.push_back(argv[i]);
        }

    } else {
        printf("usage: %s <service-type-0> <service-type-1> ... <service-type-n>\n", argv[0]);
        exit(0);
    }

    try {
        ServiceDiscovery service;
        service.removedComponentConnect(sigc::ptr_fun(removeService));
        service.addedComponentConnect(sigc::ptr_fun(addService));
        
        service.listenOn(domains);

        printf("Press ENTER to Exit\n");
        char buffer[2];
        fgets(buffer,2,stdin);
    
        return 0;
    } catch(const std::runtime_error& e)
    {
        fprintf(stderr, "Execution failed with: %s\n", e.what());
        return -1;
    }
}
