#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include <vector>
#include <semaphore.h>
#include <service_discovery/client.h>
#include <service_discovery/service_browser.h>
#include <service_discovery/service_event.h>
#include <service_discovery/service_pattern.h>
#include <service_discovery/local_service.h>

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
 * namespace sd = servicediscovery;
 * void removeCallback(dc::ServiceEvent e)
 * {
 * 
 *    // what to perform when component has been added
 *    sd::ServiceConfiguration configuration = e.getServiceConfiguration();
 *
 *    std::string serviceName = configuration.getName();
 *    std::string labelData = configuration.getDescription("my-label");
 *    ...
 * }
 * void addCallback(sd::ServiceEvent e)
 * {
 *   // what to perform when a component has been added
 * }
 * std::string someServiceData;
 * ServiceDiscovery::ServiceDiscovery service;
 * std::string serviceName = "ModuleA";
 * std::string serviceType = "_module._tcp"
 * sd::ServiceConfiguration conf(someName, serviceType);
 * conf.setDescription("my-label","data-associated-with-label");
 * service.addedComponentConnect(sigc::mem_fun(*this, &addCallback));
 * service.removedComponentConnect(sigc::mem_fun(*this, &removeCallback));
 * service.start(conf);
 *
 * std::vector<sd::ServiceDescription>
 * service.findServices(SearchPattern("my-component-name"));
 * 
 * @endverbatim
 */
class ServiceDiscovery : public sigc::trackable
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

        /**
        * Associate the service discovery object with a local service
        * \param conf Service configuration
        */
	void start(const ServiceConfiguration& conf);

        /**
        * Use the service discovery object only to listen for a list
        * of given service types
        * \param types list of types, e.g. whereas types is of format _test._tcp
        */
        void listenOn(const std::vector<std::string>& types);

        /**
        * Update a service description that has been associated with the current  service discovery
        * \param description New service description that will be published
        */
        void update(const ServiceDescription& desc);

        /**
        * Update all known services of the given name using the associated description
        * \param servicename Name of the service
        * \param description New description of the service
        */
        static bool update(const std::string& servicename, const ServiceDescription& description);
        
        /**
        * Retrieve the service description for a given service
        * \param servicename
        */
        static ServiceDescription getServiceDescription(const std::string& servicename);

        /**
        * Stop the listening and publishing activities of the service discovery object
        */
	void stop();

        /**
        * Get a list of service descriptions for services that can be updated via this method. 
        */
        static std::vector<ServiceDescription> getUpdateableServices();

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

        ServiceConfiguration getConfiguration() {
            return mLocalService->getConfiguration(); 
        }

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

        enum Mode { NONE = 0, LISTEN_ONLY = 1, PUBLISH = 2};

	/**
	* Added service
	*/
	void addedService(const RemoteService& service);

	/**
	* Removed service
	*/
	void removedService(const RemoteService& service);

        /**
         * Update service descriptions
         */
        void updatedService(const RemoteService& service);

	sigc::signal<void, ServiceEvent> ServiceAddedSignal;
	sigc::signal<void, ServiceEvent> ServiceRemovedSignal;

	sem_t added_component_sem;
	sem_t removed_component_sem;
        sem_t updated_component_sem;

	List<ServiceDescription> mServices;
	static sem_t services_sem;

        bool mPublished;

        // Seems to allow better thread safety and handling of the dbus if 
        // every browser is started with the same client object
        //
        // Map service type such as _myservice._tcp to a corresponding browser
	std::map<std::string, ServiceBrowser*> mBrowsers;

        // There can be only a single local service and a single
        // local configuration	
	LocalService* mLocalService;
	ServiceConfiguration mLocalConfiguration;

        Mode mMode;

        static std::vector<ServiceDiscovery*> msServiceDiscoveries;

        // Time to wait for a local service to be seen
	// currently set to 60 s
	int mTimeout;

};

} // end namespace servicediscovery

#endif
