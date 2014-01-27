/**
 * @file
 * a utils source file for publishing a service with the service_discovery implementation
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace servicediscovery::avahi;

int main(int argc, char** argv)
{

    std::string domain, serviceName;

    if(argc == 3)
    {
        domain = argv[1];
        serviceName = argv[2];

    } else {
        printf("usage: %s <service-type> <service-name>\n", argv[0]);
        exit(0);
    }

    ServiceDiscovery service;
    ServiceConfiguration configuration(serviceName, domain);
    service.start(configuration);
    
    printf("Press ENTER to Exit\n");
    char buffer[2];
    fgets(buffer,2,stdin);
    
    return 0;
}
