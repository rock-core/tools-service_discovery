#include "ServiceDiscovery.h"

namespace dfki 
{ 
namespace communication 
{

static afLoggingWrapper logger ("ServiceDiscovery");

ServiceDiscovery::ServiceDiscovery() : conf("", "", "")
{
	client = NULL;
	localserv = NULL;
	browser = NULL;
	started = false;
	configured = false;	

	if (
			sem_init(&services_sem,0,1) == -1
				||
			sem_init(&added_component_sem,0,1) == -1
				||
			sem_init(&removed_component_sem,0,1) == -1
		) {
		logger.log(FATAL, "Semaphore initialization failed");
		throw 1;
	}
	
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
	browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
	browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
	
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

	sem_wait(&services_sem);
	services.clear();
	sem_post(&services_sem);
	
	started = false;
}

void ServiceDiscovery::addedService(afRemoteService rms)
{
	try {
		OrocosComponentRemoteService orms(rms);

        // If the name of the service is unknown, it will be added and 
        // the signal will be omitted.
        if(services.find(rms.getName()) == services.end()) // Not found.
        {
            sem_wait(&services_sem);
			services.insert(std::pair<std::string, OrocosComponentRemoteService>
                    (rms.getName(), orms));
			sem_post(&services_sem);
			sem_wait(&added_component_sem);
			OrocosComponentAddedSignal.emit(orms);
			sem_post(&added_component_sem);
        }
	} catch(OCSException e) {
		logger.log(WARN, "Failed to create orocos component service from remote service object");
	}
}

void ServiceDiscovery::removedService(afRemoteService rms)
{
	try {
		OrocosComponentRemoteService orms(rms);	
		if (services.find(rms.getName()) != services.end()) {
			sem_wait(&removed_component_sem);
			OrocosComponentRemovedSignal.emit(orms);
			sem_post(&removed_component_sem);
			sem_wait(&services_sem);
			services.erase(orms.getName());
			sem_post(&services_sem);
		}
	} catch(OCSException e) {
		
	}

}

std::vector<OrocosComponentRemoteService> ServiceDiscovery::findServices(SearchPattern pattern)
{
	std::vector<OrocosComponentRemoteService> res;
	std::map<std::string, OrocosComponentRemoteService>::iterator it;
	sem_wait(&services_sem);
	for (it = services.begin() ; it != services.end() ; it++) {
		size_t found;
		if (pattern.name != "") {
			found = it->second.getName().find(pattern.name);
			if (found != std::string::npos) {
				res.push_back(it->second);
				continue;
			}
		}
		if (pattern.txt != "") {
			std::list<std::string>::iterator its;
			std::list<std::string> strlst = it->second.getStringList();
			for (its = strlst.begin() ; its != strlst.end() ; its++) {
				found = its->find(pattern.txt);
				if (found != std::string::npos) {
					res.push_back(it->second);
					break;
				}			
			}
		}
	}
	sem_post(&services_sem);
	return res;
}

} // namespace communication
} // namespace dfki
