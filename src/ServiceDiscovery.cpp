#include "ServiceDiscovery.h"

namespace dfki { namespace communication {

ServiceDiscovery::ServiceDiscovery() : conf("", "", "")
{
	started = false;
	configured = false;	
}

void ServiceDiscovery::start()
{
	if (started) {
		throw already_started;
	}
	
	if (!configured) {
		throw not_configured;
	}
	
	client = new afAvahiClient();
	browser = new afServiceBrowser(client, conf.avahi_type);
	
	localserv = new OrocosComponentLocalService(client, conf.name, conf.avahi_type, conf.avahi_port, conf.IOR, conf.stringlist, conf.ttl);
	
	client->dispatch();
	
	//TODO: make it resistant to client failures
	started = true;
	
}

void ServiceDiscovery::configure(const struct Configuration& configuration) {
	conf = configuration;
	configured = true;
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
