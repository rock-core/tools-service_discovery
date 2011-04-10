#include "service_discovery.h"

namespace servicediscovery {

static LoggingWrapper logger ("ServiceDiscovery");

ServiceDiscovery::ServiceDiscovery() 
{
	client = NULL;
	localserv = NULL;
	started = false;
        mode = NONE;

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
	localConfiguration = conf;

	if (started) {
		throw already_started;
	}
	
	client = new Client();
        ServiceBrowser* browser = new ServiceBrowser(client, conf.getType());

	browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
	browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));

	browsers.push_back(browser);
	localserv = new ServicePublisher(client, conf);
	
	client->dispatch();
	
	//TODO: make it resistant to client failures
	started = true;

        mode = PUBLISH;
}

void ServiceDiscovery::listenOn(const std::vector<std::string>& types)
{
    if(!started)
    {
        client = new Client();
    }

    std::vector<std::string>::const_iterator it;
    for(it = types.begin(); it != types.end(); it++)
    {
        ServiceBrowser* browser = new ServiceBrowser(client, *it);
        browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
        browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
        browsers.push_back(browser);
    }

    if(!started)
    {
	client->dispatch();
	//TODO: make it resistant to client failures
	started = true;
    }

    if(mode == NONE)
        mode = LISTEN_ONLY;
}

void ServiceDiscovery::stop()
{
	if (client)
		client->stop();

	sem_wait(&services_sem);
	services.clear();
	sem_post(&services_sem);

        std::vector<ServiceBrowser*>::iterator it;
        for(it = browsers.begin(); it != browsers.end(); it++)
        {
                if(*it)
                {
                    delete *it;
                    *it = NULL;
                }

	}

	if (localserv) {
		delete localserv;
		localserv = NULL;
	}
	
	if (client) {
		delete client;
		client = NULL;		
	}
	
	started = false;
}

//TODO: ServiceEvent should be RemoteService and ServiceDescription
void ServiceDiscovery::addedService(const RemoteService& service)
{

	ServiceEvent event(service);
	ServiceDescription foundServiceDescription = event.getServiceDescription();

	if (mode == LISTEN_ONLY || (localConfiguration.getServiceDescription() != foundServiceDescription) )
	{
			sem_wait(&services_sem);
			services.push_back( foundServiceDescription );
			sem_post(&services_sem);

			sem_wait(&added_component_sem);
			ServiceAddedSignal.emit( event );
			sem_post(&added_component_sem);

                        logger.log(INFO, "Added service %s\n", service.getName().c_str());
	}
}

void ServiceDiscovery::removedService(const RemoteService& service)
{
	ServiceEvent event(service);
	ServiceDescription serviceDescription = event.getServiceDescription(); 

	sem_wait(&services_sem);
	List<ServiceDescription>::iterator it = services.find(serviceDescription);

	if (it != services.end())	
	{
	        sem_wait(&removed_component_sem);
		ServiceRemovedSignal.emit( event );
		sem_post(&removed_component_sem);

		services.erase(it);
                logger.log(INFO, "Removed service %s\n", service.getName().c_str());
	}

	sem_post(&services_sem);

}

std::vector<std::string> ServiceDiscovery::getServiceNames()
{
	std::vector<std::string> names;
	List<ServiceDescription>::iterator it;
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
	List<ServiceDescription>::iterator it;
	sem_wait(&services_sem);
	for (it = services.begin() ; it != services.end() ; it++) {

		size_t found;
		ServiceDescription description = *it;
		if(pattern.name == "")
		{
			res.push_back(description);
			continue;
		}

		if(pattern.name == description.getName())
		{
			res.push_back(description);
			break;
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

std::vector<ServiceDescription> ServiceDiscovery::findServices(const ServicePattern& pattern,         
    std::string name_space)
{
  std::vector<ServiceDescription> result;
  List<ServiceDescription>::iterator it;
  sem_wait(&services_sem);

	for (it = services.begin(); it != services.end(); it++) {
    ServiceDescription description = *it;
    std::string serviceprop = description.getDescription("service");
  
    size_t namespace_begin = serviceprop.find_first_of(":") + 1;
    
    if((name_space == "*" || serviceprop.compare(namespace_begin, name_space.size(), name_space) == 0) 
        && pattern.matchDescription(description) ) {
      result.push_back(description);
    }
	}

	sem_post(&services_sem);
	return result;
}

} // end namespace servicediscovery
