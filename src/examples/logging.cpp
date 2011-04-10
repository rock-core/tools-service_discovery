#include <service_discovery/service_discovery.h>

namespace dc = servicediscovery;

int main (int argc, char const* argv[])
{
	dc::Client client;
	dc::ServiceBrowser sbrowser(&client, "_rimres._tcp");
	dc::LocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, std::list<std::string>());
	client.dispatch();
	sleep(4);
	client.stop();
	return 0;
}
