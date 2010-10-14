#include "ServiceDiscovery.h"

#include <cmath>

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

		if(pattern.name == description.getName())
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

std::vector<ServiceDescription> ServiceDiscovery::findServices(const ServicePattern& pattern) 
{
  std::vector<ServiceDescription> result;
  afList<ServiceDescription>::iterator it;
  sem_wait(&services_sem);

	for (it = services.begin(); it != services.end(); it++) {
    ServiceDescription description = *it;

    if( pattern.matchDescription(description) )
      result.push_back(description);
	}

	sem_post(&services_sem);
	return result;
}

bool ServiceDiscovery::PositionPattern::matchDescription(const ServiceDescription& service) const
{
  std::string position = service.getDescription("position");

  if(position.empty())
    return false;

  // Parse position into coordinate buffer via STL
  int coordinate[3];

  size_t begin = 0, comma;
  std::string str;

  for(int i = 0; i < 3; i++)
  {
    comma = position.find(",", begin);

    if(comma == std::string::npos && i != 2)
      return false;

    if(i == 2)
      str = position.substr(begin);
    else
      str = position.substr(begin, comma - begin);

    begin = comma + 1;    

    coordinate[i] = atoi(str.c_str());

    if(coordinate[i] == 0 && str != "0")
      return false;
  }

  // calculate euclidean distance between two points
  int dx = coordinate[0] - this->x;
  int dy = coordinate[1] - this->y;
  int dz = coordinate[2] - this->z;
  
  return sqrt(dx * dx + dy * dy + dz * dz) <= this->distance;
}

bool ServiceDiscovery::PropertyPattern::matchDescription(const ServiceDescription& service) const
{
  if(label == "*") {
    std::vector<std::string>::iterator it;
    std::vector<std::string> labellist = service.getLabels();

    for(it = labellist.begin(); it != labellist.end(); it++) {
      std::string desc = service.getDescription(*it);

      if(desc.find(description) != std::string::npos)
        return true;
    }

    // label = "*", description = "*" should return all found services    
    return (description == "*");
  }

  std::string desc = service.getDescription(label);

  if(desc.empty())
    return false;

  return (description == "*") || desc.find(description) != std::string::npos;
}

}}
