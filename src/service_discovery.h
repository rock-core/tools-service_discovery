#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include <vector>
#include <semaphore.h>
#include <service_discovery/client.h>
#include <service_discovery/service_browser.h>
#include <service_discovery/service_event.h>
#include <service_discovery/service_publisher.h>
#include <service_discovery/service_pattern.h>

namespace servicediscovery { 

enum SDException {
	already_started,
	not_configured
};

/**
 * @class ServiceDiscovery
 * @brief
 * A wrapper class for the avahi service discovery
 * @verbatim
 * namespace dc = rock::communication;
 * void removeCallback(dc::ServiceEvent e)
 * {
 * 
 *    // what to perform when component has been added
 *    dc::ServiceDescription sd = e.getServiceDescription();
 *
 *    std::string serviceName = sd.getName();
 *    std::string labelData = sd.getDescription("my-label");
 *    ...
 * }
 * void addCallback(dc::ServiceEvent e)
 * {
 *   // what to perform when a component has been added
 * }
 * std::string someServiceData;
 * ServiceDiscovery::ServiceDiscovery;
 * std::string serviceName = "ModuleA";
 * std::string serviceType = "_module._tcp"
 * ServiceDiscovery::Configuration conf(someName, serviceType);
 * conf.setDescription("my-label","data-associated-with-label");
 * service.addedComponentConnect(signc::mem_fun(*this, &addCallback));
 * service.removeComponentConnect(signc::mem_fin(*this, &removeCallback));
 * service.start(conf);
 *
 * std::vector<ServiceDescription>
 * service.findServices(SearchPattern("my-component-name"));
 * 
 * @endverbatim
 */
class ServiceDiscovery : public sigc::trackable
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

	void start(const ServiceConfiguration& conf);

        void listenOn(const std::vector<std::string>& types);

	void stop();

	/**
	 * Use the SearchPattern with name to search for service name and txt for txt records. both are "OR"-ed
	 */
	struct SearchPattern 
	{
		SearchPattern(std::string name) : txt("") {
			this->name = name;
		}

		SearchPattern(std::string name, std::string txt) {
			this->name = name;
			this->txt = txt;
		}

		SearchPattern(std::string name, std::string label, std::string txt)
		{
			this->name = name;
			this->label = label;
			this->txt = txt;
		}

		std::string name;
		std::string label;
		std::string txt;
	};
		
	std::vector<ServiceDescription> findServices(SearchPattern pattern);

        std::vector<ServiceDescription> findServices(const ServicePattern& pattern, 
                std::string name_space = "*");
	
	std::vector<std::string> getServiceNames();

	void addedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		sem_wait(&added_component_sem);
		ServiceAddedSignal.connect(slot);
		sem_post(&added_component_sem);
	}

	void removedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		sem_wait(&removed_component_sem);
		ServiceRemovedSignal.connect(slot);
		sem_post(&removed_component_sem);
	}

private:

        enum Mode { NONE = 0, LISTEN_ONLY, PUBLISH };

	/**
	* Added service
	*/
	void addedService(const RemoteService& service);

	/**
	* Removed service
	*/
	void removedService(const RemoteService& service);

	sigc::signal<void, ServiceEvent> ServiceAddedSignal;
	sigc::signal<void, ServiceEvent> ServiceRemovedSignal;

	sem_t added_component_sem;
	sem_t removed_component_sem;

	List<ServiceDescription> services;
	sem_t services_sem;

	bool started;

	Client* client;
	std::vector<ServiceBrowser*> browsers;

        // There can be only a single publisher and a single
        // local configuration	
	ServicePublisher* localserv;
	ServiceConfiguration localConfiguration;

        Mode mode;

};

} // end namespace servicediscovery

#endif
