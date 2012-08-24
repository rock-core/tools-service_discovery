
#include <service_discovery/service_discovery.h>
#include <stdlib.h>

using namespace std;
using namespace servicediscovery;

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

	ServiceDiscovery servdesc2;
	ServiceConfiguration conf2("SampleComponent2" ,"_sd_test._tcp");
	conf.setDescription("IOR", IOR);
	servdesc2.start(conf2);

        std::vector<std::string> types;
        types.push_back("_xgrid._tcp");
        servdesc.listenOn(types);
	
	sleep(5);
	std::vector<ServiceDescription> services =  servdesc.findServices();
	std::cout << " - Found services: " << std::endl;
        std::vector<ServiceDescription>::iterator it = services.begin();
        for(;it != services.end(); it++)
            std::cout << it->getName() << std::endl;
		
	servdesc.stop();	
	
	sleep(3);
	servdesc2.stop();
	
	return 0;
}
