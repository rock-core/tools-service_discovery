#include "ServiceDiscovery.h"

namespace dfki { namespace communication {

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
	
	localserv = new OrocosComponentLocalService(client, conf.name, conf.avahi_type, conf.avahi_port, conf.IOR, conf.stringlist, conf.ttl);
	
	client->dispatch();
	
	//TODO: make it resistant to client failures
	started = true;
	
}

void ServiceDiscovery::stop()
{
	client->stop();
	
	delete localserv;
	delete browser;
	delete client;
	
	started = false;
}

}}
