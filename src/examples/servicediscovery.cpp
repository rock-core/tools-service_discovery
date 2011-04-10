
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
	ServiceConfiguration conf("SampleComponent","_rimres._tcp");
	conf.setDescription("IOR",IOR);

	ServiceDiscovery servdesc2;
	ServiceConfiguration conf2("SampleComponent2" ,"_rimres._tcp");
	conf.setDescription("IOR", IOR);

	servdesc.start(conf);
	
	sleep(3);
	servdesc2.start(conf2);
	
	sleep(5);
	std::vector<ServiceDescription> servs =  servdesc.findServices(ServiceDiscovery::SearchPattern("Sample"));
	std::cout << " - Found services: " << servs.size() << std::endl;
		
	servdesc.stop();	
	
	sleep(3);
	servdesc2.stop();
	
	return 0;
}
