#include "ServiceDiscovery.hpp"
#include <stdexcept>
#include <map>
#include <boost/timer.hpp>
#include <base-logging/Logging.hpp>

namespace servicediscovery {
namespace avahi {

std::vector<ServiceDiscovery*> ServiceDiscovery::msServiceDiscoveries;
boost::mutex ServiceDiscovery::mServicesMutex;

ServiceDiscovery::ServiceDiscovery()
    : mPublished(false)
{
        mLocalService = NULL;
        mMode = NONE;

        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);
            msServiceDiscoveries.push_back(this);
        }

        Client* client = Client::getInstance();
        client->addObserver(this);
}

ServiceDiscovery::~ServiceDiscovery()
{
        Client* client = Client::getInstance();
        client->removeObserver(this);

        stop();

        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);

            std::vector<ServiceDiscovery*>::iterator it;
            it = std::find(msServiceDiscoveries.begin(), msServiceDiscoveries.end(), this);
            if(it != msServiceDiscoveries.end())
                msServiceDiscoveries.erase(it);
        }
}

bool ServiceDiscovery::update(const std::string& name, const ServiceDescription& description)
{
        // search all service discoveries in the same process for the service with the given name
        bool success = false;
        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);
            std::vector<ServiceDiscovery*>::iterator it;
            for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
            {
                try {
                    ServiceConfiguration conf = (*it)->getConfiguration();
                    if(conf.getName() == name)
                    {
                        (*it)->update(description);
                        success = true;
                        break;
                    }
                } catch(...)
                {
                    // ignore errors
                }
            }
        }
        return success;
}

ServiceDescription ServiceDiscovery::getServiceDescription(const std::string& name)
{
        bool found = false;
        ServiceDescription sd("");
        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);

            std::vector<ServiceDiscovery*>::iterator it;
            for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
            {
                try {
                    ServiceConfiguration conf = (*it)->getConfiguration();
                    if(conf.getName() == name)
                    {
                        sd = conf;
                        found = true;
                        break;
                    }
                } catch(...)
                {
                    // ignore errors
                }
            }
        }

        if(!found)
        {
            char buffer[512];
            snprintf(buffer, 512, "Could not find service: %s\n", name.c_str());
            throw std::runtime_error(std::string(buffer));
        }

        return sd;
}

void ServiceDiscovery::registerCallbacks(const std::string& serviceName, const sigc::slot<void, ServiceEvent>& serviceAdded, const sigc::slot<void, ServiceEvent>& serviceRemoved)
{
        bool found = false;
        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);
            std::vector<ServiceDiscovery*>::iterator it;
            for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
            {
                ServiceDiscovery* serviceDiscovery = *it;
                try {
                    if(serviceDiscovery->getConfiguration().getName() == serviceName)
                    {
                        serviceDiscovery->addedComponentConnect(serviceAdded);
                        serviceDiscovery->removedComponentConnect(serviceRemoved);
                        found = true;
                    }
                } catch(...)
                {
                    // ignore errors
                }
            }
        }

        if(!found)
        {
            char buffer[512];
            snprintf(buffer, 512, "Could not find service: %s\n", serviceName.c_str());
            throw std::runtime_error(std::string(buffer));
        }
}

std::map<std::string, ServiceDescription> ServiceDiscovery::getVisibleServices(const SearchPattern& pattern)
{
        std::map<std::string, ServiceDescription> descriptions;
        boost::unique_lock<boost::mutex> lock(mServicesMutex);

        std::vector<ServiceDiscovery*>::iterator it = msServiceDiscoveries.begin();
        for(; it != msServiceDiscoveries.end(); ++it)
        {
            LOG_DEBUG("Find services");
            std::vector<ServiceDescription> services = (*it)->_findServices(pattern);
            std::vector<ServiceDescription>::iterator serviceIt = services.begin(); 
            for(; serviceIt != services.end(); ++serviceIt)
            {
                if(descriptions.count(serviceIt->getName()) == 0)
                {
                    descriptions[serviceIt->getName()] = *serviceIt;
                } else {
                    LOG_DEBUG("Service already known: '%s'", serviceIt->getName().c_str());
                }
            }
        }

        return descriptions;
}


std::vector<ServiceDescription> ServiceDiscovery::getUpdateableServices()
{
	std::vector<ServiceDescription> serviceList;
	boost::unique_lock<boost::mutex> lock(mServicesMutex);

	std::vector<ServiceDiscovery*>::iterator it;
	for(it = msServiceDiscoveries.begin(); it != msServiceDiscoveries.end(); it++)
	{
		try {
			ServiceConfiguration conf = (*it)->getConfiguration();
			serviceList.push_back(conf);

	} catch(...)
	{
		// ignore errors
	}
	}
	
	return serviceList;
}

void ServiceDiscovery::start(const ServiceConfiguration& conf)
{
	UniqueClientLock lock;
	_start(conf);
}

void ServiceDiscovery::listenOn(const std::vector<std::string>& types)
{
	UniqueClientLock lock;
	_listenOn(types);
}



void ServiceDiscovery::update(const ServiceDescription& desc) 
{
    if(mLocalService != NULL) 
    {
        std::list<std::string> raw_desc = desc.getRawDescriptions();
        UniqueClientLock lock;
        mLocalService->updateStringList(raw_desc);

        LOG_INFO("Updated local service: %s", mLocalService->getName().c_str());
    } else {
        LOG_FATAL("Service Discovery tries to update a description on a non-started local service.\n");
        throw std::runtime_error("Service Discovery tries to update a description on a non-started local service");
    }
}

//TODO: ServiceEvent should be RemoteService and ServiceDescription
void ServiceDiscovery::addedService(const RemoteService& service)
{
        ServiceEvent event(service);
        ServiceConfiguration remoteConfig = service.getConfiguration();

        if(mLocalService && mMode == PUBLISH)
        {
            ServiceConfiguration localConfig = mLocalService->getConfiguration();

            if ( localConfig.getName() == remoteConfig.getName() && localConfig.getType() == remoteConfig.getType())
            {
                LOG_INFO("Service published: %s", service.getName().c_str());
                mPublished = true;
            }
        }

        {
            boost::unique_lock<boost::mutex> lock(mServicesMutex);
            ServiceDescription configuration = service.getConfiguration();
            LOG_INFO("+ %s type: %s", configuration.getName().c_str(), configuration.getType().c_str());
            mServices.push_back( configuration );
        }

        {
            boost::unique_lock<boost::mutex> lock(mAddedComponentMutex);
            ServiceAddedSignal.emit( event );
        }
}

std::vector<ServiceDescription> ServiceDiscovery::_findServices(const SearchPattern& pattern) const
{
	std::vector<ServiceDescription> res;
	List<ServiceDescription>::const_iterator it;
	for (it = mServices.begin() ; it != mServices.end() ; it++) 
	{
		ServiceDescription description = *it;
                if(pattern.isMatching(description))
                {
			res.push_back(description);
                        continue;
                }
	}

        return res;
}


void ServiceDiscovery::updatedService(const RemoteService& service)
{
    ServiceDescription desc = service.getConfiguration();

    boost::unique_lock<boost::mutex> lock(mServicesMutex);

    List<ServiceDescription>::iterator it;

    for(it = mServices.begin(); it != mServices.end(); it++) 
    {
       if( desc.compareWithoutTXT(*it) ) 
       {
           std::vector<std::string> labels = desc.getLabels();

           for(unsigned int i = 0; i < labels.size(); i++) 
           {
               it->setDescription(labels[i], desc.getDescription(labels[i]));
           }

           LOG_INFO("Updated service: %s", service.getName().c_str());
       }
    }
}


void ServiceDiscovery::removedService(const RemoteService& service)
{
	ServiceEvent event(service);
	ServiceDescription serviceDescription = event.getServiceConfiguration(); 

	boost::unique_lock<boost::mutex> lock(mServicesMutex);
	List<ServiceDescription>::iterator it = mServices.find(serviceDescription);

	if (it != mServices.end())	
	{
		{
			boost::unique_lock<boost::mutex> removalLock(mRemovedComponentMutex);
			ServiceDescription configuration = *it;
			LOG_INFO("- %s type: %s", configuration.getName().c_str(), configuration.getType().c_str());
			ServiceRemovedSignal.emit( event );
		}

		mServices.erase(it);
	}
}

std::vector<std::string> ServiceDiscovery::getServiceNames()
{
	std::vector<std::string> names;
	List<ServiceDescription>::iterator it;
	boost::unique_lock<boost::mutex> lock(mServicesMutex);

	for (it = mServices.begin() ; it != mServices.end() ; it++) 
	{
		names.push_back(it->getName());
	}
	return names;
}

bool ServiceDiscovery::isRunning() const
{
    if(mLocalService)
    {
        return true;
    }

    return false;
}

ServiceConfiguration ServiceDiscovery::getConfiguration() const
{
    if(mMode == LISTEN_ONLY)
    {
        throw std::runtime_error("service_discovery::avahi::ServiceDisccovery: Get configuration cannot be called for a service started in listen only mode");
    } else {
        return mLocalServiceConfiguration;
    }
}

std::vector<ServiceDescription> ServiceDiscovery::findServices(const SearchPattern& pattern) const
{
	std::vector<ServiceDescription> res;
	boost::unique_lock<boost::mutex> lock(mServicesMutex);
	res = _findServices(pattern);
	return res;
}

std::vector<ServiceDescription> ServiceDiscovery::findServices(const ServicePattern& pattern, const std::string& name_space) const
{
    std::vector<ServiceDescription> result;
    List<ServiceDescription>::const_iterator it;

    boost::unique_lock<boost::mutex> lock(mServicesMutex);

    for (it = mServices.begin(); it != mServices.end(); it++) 
    {
        ServiceDescription description = *it;
        std::string serviceprop = description.getDescription("service");
      
        size_t namespace_begin = serviceprop.find_first_of(":") + 1;
        
        if((name_space == "*" || serviceprop.compare(namespace_begin, name_space.size(), name_space) == 0) 
            && pattern.matchDescription(description) ) 
        {
          result.push_back(description);
        }
    }

    return result;
}

void ServiceDiscovery::update(const ClientObserver::Event& event)
{
    switch(event.type)
    {
        case ClientObserver::DISCONNECTED:
            LOG_DEBUG("Client disconnected");
            _stop(false /*lock_client*/, false);
            break;
        case ClientObserver::RECOVERED:
            LOG_DEBUG("Client recovered");
            reconnect();
            break;
        default:
            // do nothing
            break;
    }
}

void ServiceDiscovery::reconnect()
{
    LOG_DEBUG("Reconnecting");
    switch(mMode)
    {
        case LISTEN_ONLY:
            _listenOn(mListenTypes);
            break;
        case PUBLISH:
            _start(mLocalServiceConfiguration);
            break;
        case NONE:
            // do nothing
            break;
    }
}

void ServiceDiscovery::_start(const ServiceConfiguration& conf)
{

    if( mLocalService != NULL ) 
    {
        LOG_FATAL("Service Discovery instance tries to start a second local service: %s", conf.getName().c_str());
        throw std::runtime_error("Service Discovery instance tries to start a second local service");
    }

    mMode = PUBLISH;

    Client* client = Client::getInstance();

    // Register browser if it does not exist yet for this type
    if( mBrowsers.count(conf.getType()) == 0)
    {
        // Register browser
        LOG_INFO("Adding service browser for type: %s", conf.getType().c_str());
        ServiceBrowser* browser = 0;
        {
            browser = new ServiceBrowser(client, conf.getType());
            browser->serviceUpdatedConnect(sigc::mem_fun(this, &ServiceDiscovery::updatedService));
            browser->serviceAddedConnect(sigc::mem_fun(this, &ServiceDiscovery::addedService));
            browser->serviceRemovedConnect(sigc::mem_fun(this, &ServiceDiscovery::removedService));
        }
        mBrowsers[conf.getType()] = browser;
    }

    LOG_INFO("Creating local service %s", conf.getName().c_str());
    mLocalService = new LocalService(client, conf.getName(), conf.getType(), conf.getPort(), conf.getRawDescriptions(), conf.getTTL(), true);

    // Set the local configuration object
    mLocalServiceConfiguration = conf;
}

void ServiceDiscovery::_listenOn(const std::vector<std::string>& types)
{
    if(mMode == NONE)
    {
        mMode = LISTEN_ONLY;
        mListenTypes = types;
    }

    Client* client = Client::getInstance();

    std::vector<std::string>::const_iterator it;
    for(it = types.begin(); it != types.end(); ++it)
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

void ServiceDiscovery::stop()
{
    _stop(true, true);
}

void ServiceDiscovery::_stop(bool lock_client, bool wait_for_unpublished)
{
    if (mLocalService)
    {
        {
            if(lock_client)
            {
                mLocalService->unpublish();
            } else {
                mLocalService->_unpublish();
            }
        }

        while(wait_for_unpublished && mLocalService->published())
        {
            // waiting for local service to be unpublished
            usleep(100);
        }
        delete mLocalService;
        mLocalService = NULL;
    }

    {
        if(lock_client)
        {
            UniqueClientLock lock;
            cleanupBrowsers();
        } else {
            cleanupBrowsers();
        }
    }

    {
        boost::unique_lock<boost::mutex> lock(mServicesMutex);
        mServices.clear();
    }
}

void ServiceDiscovery::cleanupBrowsers()
{
	// Delete all browsers that are associated with this service discovery
	std::map<std::string, ServiceBrowser*>::iterator it;
	for(it = mBrowsers.begin(); it != mBrowsers.end(); ++it)
	{
	        if(it->second)
	        {
	            delete it->second;
	            it->second = NULL;
	        }
	}
	mBrowsers.clear();
}

} // end namespace avahi
} // end namespace servicediscovery
