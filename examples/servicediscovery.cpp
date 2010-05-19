
#include <service-discovery/ServiceDiscovery.h>
#include <stdlib.h>

using namespace std;
using namespace dfki::communication;

void addedOrocosComponent(OrocosComponentRemoteService rms)
{
	std::cout << " -=- OROCOS Component found: " << rms.getName() << std::endl;
}

int main (int argc, char const* argv[])
{

#ifdef AVAHI_ALLOW_LOOPBACK
	std::cout << "AVAHI_ALLOW_LOOPBACK defined\n";
#else
	std::cout << "AVAHI_ALLOW_LOOPBACK undefined\n";
#endif

	string IOR = 	
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890");
		
	ServiceDiscovery servdesc;
	ServiceDiscovery::Configuration conf(IOR, "SampleComponent" ,"_rimres._tcp");
	servdesc.OrocosComponentAddedSignal.connect(sigc::ptr_fun(addedOrocosComponent));
	servdesc.configure(conf);


	ServiceDiscovery servdesc2;
	ServiceDiscovery::Configuration conf2(IOR, "SampleComponent2" ,"_rimres._tcp");
	servdesc2.configure(conf2);
	
	servdesc.start();
	
	sleep(3);
	servdesc2.start();
	
	sleep(5);
	std::vector<OrocosComponentRemoteService> servs =  servdesc.findServices(ServiceDiscovery::SearchPattern("Sample"));
	std::cout << " - Found services: " << servs.size() << std::endl;
		
	servdesc.stop();	
	
	sleep(3);
	servdesc2.stop();
	
	return 0;
}
