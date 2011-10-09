#include "service_discovery.h"
#include <boost/timer.hpp>
#include <stdexcept>
#include <base/logging.h>

namespace servicediscovery {

std::vector<ServiceDiscovery*> ServiceDiscovery::msServiceDiscoveries;
sem_t ServiceDiscovery::services_sem;

ServiceDiscovery::ServiceDiscovery() 
	: mPublished(false)
	, mTimeout(60)
{
	mLocalService = NULL;
        mMode = NONE;

	if (
		sem_init(&services_sem,0,1) == -1
			||
		sem_init(&added_component_sem,0,1) == -1
			||
		sem_init(&removed_component_sem,0,1) == -1
                        ||
                sem_init(&updated_component_sem,0,1) == -1
	) {
		LOG_FATAL("Semaphore initialization failed");
		throw 1;
	}

        sem_wait(&services_sem);
        msServiceDiscoveries.push_back(this);
        sem_post(&services_sem);
}

ServiceDiscovery::~ServiceDiscovery()
{
	stop();

        sem_wait(&services_sem);
        std::vector<ServiceDiscovery*>::iterator it;
        it = std::find(msServiceDiscoveries.begin(), msServiceDiscoveries.end(), this);
        if(it != msServiceDiscoveries.end())
            msServiceDiscoveries.erase(it);
        sem_post(&services_sem);
}

bool ServiceDiscovery::update(const std::string& name, const ServiceDescription& description)
{
        // serach all service discoveries in the same process for the service with the given name
        bool success = false;
        sem_wait(&services_sem);
        std::vector<ServiceDiscovery*>::iterator it;
        for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
        {
            ServiceConfiguration conf = (*it)->getConfiguration();
            if(conf.getName() == name)
            {
                (*it)->update(description);  
                success = true;
                break;
            }
        }
        sem_post(&services_sem);

        return success;
}

ServiceDescription ServiceDiscovery::getServiceDescription(const std::string& name)
{
        bool found = false;
        ServiceDescription sd("");
        sem_wait(&services_sem);
        std::vector<ServiceDiscovery*>::iterator it;
        for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
        {
            ServiceConfiguration conf = (*it)->getConfiguration();
            if(conf.getName() == name)
            {
                sd == conf;
                found = true;
                break;
            }
        }
        sem_post(&services_sem);

        if(!found)
        {
            char buffer[512];
            sprintf(buffer, "Could not find service: %s\n", name.c_str());
            throw std::runtime_error(std::string(buffer));
        }

        return sd;
}


std::vector<ServiceDescription> ServiceDiscovery::getUpdateableServices()
{
        std::vector<ServiceDescription> serviceList;
        sem_wait(&services_sem);
        std::vector<ServiceDiscovery*>::iterator it;
        for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
        {
            ServiceConfiguration conf = (*it)->getConfiguration();
            serviceList.push_back(conf);
        }
        sem_post(&services_sem);

        return serviceList;
}

void ServiceDiscovery::start(const ServiceConfiguration& conf)
{

    if( mLocalService != NULL ) {
        LOG_FATAL("Service Discovery instance tries to start a second local service");
        throw std::runtime_error("Service Discovery instance tries to start a second local service");
    }

    mMode = PUBLISH;
    mLocalConfiguration = conf;

    Client* client = Client::getInstance();

    // Register browser if it does not exist yet for this type    
    if( mBrowsers.count(conf.getType()) == 0)
    {
        // Register browser
        LOG_INFO("Adding service browser for type: %s", conf.getType().c_str());
        ServiceBrowser* browser = new ServiceBrowser(client, conf.getType());
        browser->serviceUpdatedConnect(sigc::mem_fun(this, &ServiceDiscovery::updatedService));
        browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
        browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
        mBrowsers[conf.getType()] = browser;
    }

    LOG_INFO("Creating local service %s, waiting for it to show up in servicelist ...", conf.getName().c_str());
    mLocalService = new LocalService(client, conf.getName(), conf.getType(), conf.getPort(), conf.getRawDescriptions(), conf.getTTL(), true);

    // making sure the service can be seen before proceeding 
    boost::timer timer;	
    while(!mPublished)
    {
        if(!mPublished && timer.elapsed_min() > mTimeout)
        {
            LOG_FATAL("Timout reached: resolution of local service failed");
            throw std::runtime_error("Timeout reached: resolution of local service failed\n");
        } else if(mPublished)
        {
            break;
        }

        sleep(0.1);
    }

    LOG_INFO("Local service %s started", conf.getName().c_str());
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
            LOG_INFO("Adding service browser for type: %s",it->c_str());

            ServiceBrowser* browser = new ServiceBrowser(client, *it);
            
            browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
            browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
            browser->serviceUpdatedConnect(sigc::mem_fun(this, &ServiceDiscovery::updatedService));
            mBrowsers[*it] = browser;
        }
    }
}


void ServiceDiscovery::update(const ServiceDescription& desc) 
{
    if(mLocalService != NULL) {
        std::list<std::string> raw_desc = desc.getRawDescriptions();
        std::vector<std::string> labels = desc.getLabels();

        mLocalService->updateStringList(raw_desc);

        std::vector<std::string>::iterator it;

        for(it = labels.begin(); it != labels.end(); it++) {
            mLocalConfiguration.setDescription(*it, desc.getDescription(*it));
        }

        LOG_INFO("Updated local service: %s", mLocalService->getName().c_str());
    } else {
        LOG_FATAL("Service Discovery tries to update a description on a non-started local service.\n");
        throw std::runtime_error("Service Discovery tries to update a description on a non-started local service");
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
            LOG_INFO("Service published: %s", service.getName().c_str());
            mPublished = true;
        }

	sem_wait(&services_sem);
	mServices.push_back( service.getConfiguration() );
	sem_post(&services_sem);

	sem_wait(&added_component_sem);
	ServiceAddedSignal.emit( event );
	sem_post(&added_component_sem);
}


void ServiceDiscovery::updatedService(const RemoteService& service)
{
    ServiceDescription desc = service.getConfiguration();

    sem_wait(&services_sem);

    List<ServiceDescription>::iterator it;

    for(it = mServices.begin(); it != mServices.end(); it++) {
       if( desc.compareWithoutTXT(*it) ) {
           std::vector<std::string> labels = desc.getLabels();

           for(unsigned int i = 0; i < labels.size(); i++) {
               it->setDescription(labels[i], desc.getDescription(labels[i]));
           }

           LOG_INFO("Updated service: %s", service.getName().c_str());
       }
    }

    sem_post(&services_sem);
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
                LOG_INFO("Removed service %s", service.getName().c_str());
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
