
#include <service_discovery/ServiceDiscovery.hpp>
#include <stdlib.h>

using namespace std;
using namespace servicediscovery::avahi;

void addedComponent(ServiceDescription service)
{
	std::cout << " -=- Service found: " << service.getName() << std::endl;
}

int main (int argc, char const* argv[])
{
	string IOR = 	
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890");
		
	ServiceDiscovery servdesc;
	ServiceConfiguration conf("SampleComponent","_sd_test._tcp");
	conf.setDescription("IOR",IOR);
	servdesc.start(conf);

        printf("Retrieve description for sample component\n");
        ServiceDescription sd = ServiceDiscovery::getServiceDescription("SampleComponent");
        sleep(5);
        printf("Set new description for sample component\n");
        sd.setDescription("LOCATION","x:1,y:2,z:3");
        printf("Update description for sample component\t");
        if(ServiceDiscovery::update("SampleComponent", sd))
            printf("[OK]\n");
        else
            printf("[FAILED]\n");
        sleep (5);

	servdesc.stop();
	return 0;
}
