#include "ServiceDiscovery.h"

namespace dfki { namespace communication {

static afLoggingWrapper logger ("ServiceDiscovery");

ServiceDiscovery::ServiceDiscovery() 
{
	client = NULL;
	localserv = NULL;
	browser = NULL;
	started = false;

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

void ServiceDiscovery::start(const ServiceConfiguration& conf)
{
	localConfiguration_ = conf;

	if (started) {
		throw already_started;
	}
	
	client = new afAvahiClient();
	browser = new afServiceBrowser(client, conf.getType());

	browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
	browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));

	localserv = new ServicePublisher(client, conf);
	
	client->dispatch();
	
	//TODO: make it resistant to client failures
	started = true;
	
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

//TODO: ServiceEvent should be afRemoteService and ServiceDescription
void ServiceDiscovery::addedService(const afRemoteService& service)
{
	ServiceEvent event(service);
	ServiceDescription foundServiceDescription = event.getServiceDescription();

	if (! (localConfiguration_.getServiceDescription() == foundServiceDescription) )
	{
			sem_wait(&services_sem);
			services.push_back( foundServiceDescription );
			sem_post(&services_sem);
			sem_wait(&added_component_sem);

			ServiceAddedSignal.emit( event );

			sem_post(&added_component_sem);
	}
}

void ServiceDiscovery::removedService(const afRemoteService& service)
{
	ServiceEvent event(service);
	ServiceDescription serviceDescription = event.getServiceDescription(); 

	afList<ServiceDescription>::iterator it = services.find(serviceDescription);

	if (it != services.end())	
	{
		sem_wait(&removed_component_sem);
		ServiceRemovedSignal.emit( event );
		sem_post(&removed_component_sem);
		sem_wait(&services_sem);
		services.erase(it);
		sem_post(&services_sem);
	}

}

std::vector<std::string> ServiceDiscovery::getServiceNames()
{
	std::vector<std::string> names;
	afList<ServiceDescription>::iterator it;
	sem_wait(&services_sem);
	for (it = services.begin() ; it != services.end() ; it++) {
		names.push_back(it->getName());
	}
	sem_post(&services_sem);
	return names;
}


std::vector<ServiceDescription> ServiceDiscovery::findServices(SearchPattern pattern)
{
	std::vector<ServiceDescription> res;
	afList<ServiceDescription>::iterator it;
	sem_wait(&services_sem);
	for (it = services.begin() ; it != services.end() ; it++) {

		size_t found;
		ServiceDescription description = *it;
		if(pattern.name == "")
		{
			res.push_back(description);
			continue;
		}

		if (pattern.name == description.getName())
		{
			res.push_back(description);
			return res;
		}
		
		std::string descriptionItem = description.getDescription(pattern.label);
		if( descriptionItem != "") 
		{
			if(descriptionItem.find(pattern.txt))
			{
				res.push_back(description);
				continue;
			}

		} else if(pattern.txt != "") {

			std::vector<std::string> labels = description.getLabels();
			int labelsSize = labels.size();

			for(int i = 0; i < labelsSize; i++)
			{
				std::string descriptionItem = description.getDescription(pattern.label);
				if(descriptionItem.find(pattern.txt))
				{
					res.push_back(description);
					continue;
				}
			}
		}
	}
	sem_post(&services_sem);
	return res;
}

}}
