#include "ServiceDiscovery.h"

namespace dfki { namespace communication {

static afLoggingWrapper logger ("ServiceDiscovery");

ServiceDiscovery::ServiceDiscovery() : conf("", "", "")
{
	client = NULL;
	localserv = NULL;
	browser = NULL;
	started = false;
	configured = false;	
}

ServiceDiscovery::~ServiceDiscovery()
{
	stop();
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
	browser->afServiceAdded.connect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
	browser->afServiceRemoved.connect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
	
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
	if (client)
		client->stop();
	
	if (localserv) {
		delete localserv;
		localserv = NULL;
	}
	
	
	if (browser) {
		delete browser;
		browser = NULL;
	}
	
	
	if (client) {
		delete client;
		client = NULL;		
	}
	
	services.clear();
	
	started = false;
}

void ServiceDiscovery::addedService(afRemoteService rms)
{
	try {
		OrocosComponentRemoteService orms(rms);
		

		if (!((afService) (*localserv) == (afService) rms)) {


			services.push_back(orms);
			OrocosComponentAddedSignal.emit(orms);

		}
		
			
	} catch(OCSException e) {
		logger.log(WARN, "Failed to create orocos component service from remote service object");
	}
}

void ServiceDiscovery::removedService(afRemoteService rms)
{
	
	try {
		OrocosComponentRemoteService orms(rms);	
		afList<OrocosComponentRemoteService>::iterator it = services.find(orms);
		if (it != services.end()) {
			OrocosComponentRemovedSignal.emit(orms);
			services.erase(it);
		}
	} catch(OCSException e) {
		
	}

}

std::vector<OrocosComponentRemoteService> ServiceDiscovery::findServices(SearchPattern pattern)
{
	std::vector<OrocosComponentRemoteService> res;
	afList<OrocosComponentRemoteService>::iterator it;
	for (it = services.begin() ; it != services.end() ; it++) {
		size_t found;
		if (pattern.name != "") {
			found = (*it).getName().find(pattern.name);
			if (found != std::string::npos) {
				res.push_back(*it);
				continue;
			}		
		}
		if (pattern.txt != "") {
			std::list<std::string>::iterator its;
			std::list<std::string> strlst = (*it).getStringList();
			for (its = strlst.begin() ; its != strlst.end() ; its++) {
				found = (*its).find(pattern.txt);
				if (found != std::string::npos) {
					res.push_back(*it);
					break;
				}			
			}
		}
	}
	return res;
}

}}
