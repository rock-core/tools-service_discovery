#include "service_discovery.h"
#include <boost/timer.hpp>
#include <stdexcept>

namespace servicediscovery {

static LoggingWrapper logger ("ServiceDiscovery");

ServiceDiscovery::ServiceDiscovery()
{
	mLocalService = NULL;
        mMode = NONE;

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

        mMode = PUBLISH;
	mLocalConfiguration = conf;

        Client* client = Client::getInstance();
        
        // Register browser if it does not exist yet for this type    
        if( mBrowsers.count(conf.getType()) == 0)
        {
            // Register browser
            logger.log(INFO, "Adding service browser for type: %s", conf.getType().c_str());
            ServiceBrowser* browser = new ServiceBrowser(client, conf.getType());
            browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
            browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
            mBrowsers[conf.getType()] = browser;
        }

        logger.log(INFO, "Creating local service %s", conf.getName().c_str());
	mLocalService = new LocalService(client, conf.getName(), conf.getType(), conf.getPort(), conf.getServiceDescription().getRawDescriptions(), conf.getTTL(), true);

        // making sure it the service is seen before proceeding 
        boost::timer timer;	
        while(!mPublished)
        {
            if(!mPublished && timer.elapsed_min() > 10)
            {
                logger.log(FATAL, "Timout reached: resolution of local service failed\n");
                throw std::runtime_error("Timeout reached: resolution of local service failed\n");
            } else if(mPublished)
            {
                break;
            }

            sleep(0.1);
        }
	
        logger.log(INFO, "Local service %s started", conf.getName().c_str());
}

void ServiceDiscovery::listenOn(const std::vector<std::string>& types)
{
    if(mMode == NONE)
        mMode = LISTEN_ONLY;

    Client* client = Client::getInstance();

    std::vector<std::string>::const_iterator it;
    for(it = types.begin(); it != types.end(); it++)
    {
        std::map<std::string, ServiceBrowser*>::const_iterator browserIt;
        browserIt = mBrowsers.find(*it);
        if( browserIt == mBrowsers.end())
        {
            logger.log(INFO, "Adding service browser for type: %s\n",it->c_str());

            ServiceBrowser* browser = new ServiceBrowser(client, *it);
            
            browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
            browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
            mBrowsers[*it] = browser;
        }
    }
}

void ServiceDiscovery::stop()
{
	sem_wait(&services_sem);
	mServices.clear();
	sem_post(&services_sem);
        
        // delete all mBrowsers
        std::map<std::string, ServiceBrowser*>::iterator it;
        for(it = mBrowsers.begin(); it != mBrowsers.end(); it++)
        {
                if(it->second)
                {
                    delete it->second;
                    it->second = NULL;
                }

	}

	if (mLocalService) {
		delete mLocalService;
		mLocalService = NULL;
	}

}

//TODO: ServiceEvent should be RemoteService and ServiceDescription
void ServiceDiscovery::addedService(const RemoteService& service)
{

	ServiceEvent event(service);
        ServiceConfiguration remoteConfig = service.getConfiguration();

        if ( mMode == PUBLISH && mLocalConfiguration.getName() == remoteConfig.getName() && mLocalConfiguration.getType() == remoteConfig.getType())
	{
            logger.log(INFO, "Service published: %s\n", service.getName().c_str());
            mPublished = true;
        }

	sem_wait(&services_sem);
	mServices.push_back( service.getConfiguration() );
	sem_post(&services_sem);

	sem_wait(&added_component_sem);
	ServiceAddedSignal.emit( event );
	sem_post(&added_component_sem);
}

void ServiceDiscovery::removedService(const RemoteService& service)
{
	ServiceEvent event(service);
	ServiceDescription serviceDescription = event.getServiceConfiguration(); 

	sem_wait(&services_sem);
	List<ServiceDescription>::iterator it = mServices.find(serviceDescription);

	if (it != mServices.end())	
	{
	        sem_wait(&removed_component_sem);
		ServiceRemovedSignal.emit( event );
		sem_post(&removed_component_sem);

		mServices.erase(it);
                logger.log(INFO, "Removed service %s\n", service.getName().c_str());
	}

	sem_post(&services_sem);
}

std::vector<std::string> ServiceDiscovery::getServiceNames()
{
	std::vector<std::string> names;
	List<ServiceDescription>::iterator it;
	sem_wait(&services_sem);
	for (it = mServices.begin() ; it != mServices.end() ; it++) {
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
	for (it = mServices.begin() ; it != mServices.end() ; it++) {

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

    for (it = mServices.begin(); it != mServices.end(); it++) {
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
