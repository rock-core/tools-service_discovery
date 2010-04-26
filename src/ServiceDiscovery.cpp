

#include "ServiceDiscovery.h"

using namespace std;
using namespace dfki::communication;

ServiceDiscovery::ServiceDiscovery()
{
	started = false;	
}

void ServiceDiscovery::start()
{
	if (started) {
		return;
	}
	
	client = new afAvahiClient();
	browser = new afServiceBrowser(client, conf.avahi_type);
	
	localserv = new OrocosComponentLocalService(client, conf.name, conf.avahi_type, IOR, conf.stringlist, ttl);
	
	client->dispatch();
	
	//TODO: make it resistant to client failures
	started = true;
	
}

void ServiceDiscovery::stop()
{
	client->stop();
	
	delete localserv();
	delete browser();
	delete client();
	
	started = false;
}
