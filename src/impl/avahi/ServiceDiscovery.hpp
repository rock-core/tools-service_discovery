#ifndef _SERVICEDISCOVERY_CORE_H_
#define _SERVICEDISCOVERY_CORE_H_

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <service_discovery/impl/avahi/Client.hpp>
#include <service_discovery/impl/avahi/ServiceBrowser.hpp>
#include <service_discovery/impl/avahi/ServiceEvent.hpp>
#include <service_discovery/impl/avahi/ServicePattern.hpp>
#include <service_discovery/impl/avahi/LocalService.hpp>

namespace servicediscovery { 
namespace avahi {

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
 * std::vector<sd::ServiceDescription> services = 
 *     service.findServices(SearchPattern("my-component-name"));
 * 
 * @endverbatim
 * @detail
 * The service discovery takes advantage of the features of avahi and allows 
 * you to publish and browse for services based on existing or newly created
 * types. 
 *
 * Depending on the need, the service discovery can be used in a listening mode
 * only or in publishing mode. However, for each service you intend to publish, 
 * you have to create a new ServiceDiscovery instance. 
 *
 * This implementation support a avahi-daemon restart after you service discovery
 * instances have been created. 
 * At startup of the service discovery a running avahi-daemon is required. 
 * Nevertheless, if you need to restart the avahi-daemon after a successful start, 
 * all service browsers will be registered.
 * Any attached application has to deal with the newly emitted ServiceEvents. 
 *
 * Implementation details: 
 * All service discovery objects within the same process will access the 
 * avahi daemon through a single client connection. Thus synchronization is 
 * needed and performed using a unique lock on the client instance.
 *
 */
class ServiceDiscovery : public sigc::trackable, public ClientObserver
{

public: 
	ServiceDiscovery();

        /**
         * Default deconstructor -- will cleanup all underlying resolvers 
         * and browsers created for this instance
         */
	virtual ~ServiceDiscovery();

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
        * Update a service description that has been associated with the current service discovery
        * \param description New service description that will be published
        */
        void update(const ServiceDescription& desc);

        /**
         * Implementation of the observer function in order to 
         * react to underlying client connection changes
         * This update is called from the Client and with an
         * acquire avahi client lock
         */
        virtual void update(const ClientObserver::Event& event);

        /**
        * Stop the listening and publishing activities of the service discovery object
        */
	void stop();

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
        * Get a list of service descriptions for services that can be updated via the 
        * update(const std::string&, const ServiceDescription& ) method. 
        */
        static std::vector<ServiceDescription> getUpdateableServices();

        /**
         * Search Services using a standard search pattern
         * \return List of service descriptions of all available services, by default of all services
         * seen by this service discovery instance
         */
	std::vector<ServiceDescription> findServices( const SearchPattern& pattern = SearchPattern()) const;

        /**
         * Search services using a predefined set of service pattern
         */
        std::vector<ServiceDescription> findServices(const ServicePattern& pattern, 
                const std::string& name_space = "*") const;

        /**
         * Get list of names of services currently seen by this service discovery instance
         */
	std::vector<std::string> getServiceNames();

        /**
         * Check if service discovery is running
         */
        bool isRunning() const;


        /**
         * Get the current configuration of this service discovery if in PUBLISH mode and when
         * the service is running
         * \throws if the service discovery is in LISTEN_ONLY mode, where no service configuration
         * exists
         * \return ServiceConfiguration of a published service
         */
        ServiceConfiguration getConfiguration() const;

	void addedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		boost::unique_lock<boost::mutex> lock(mAddedComponentMutex);
		ServiceAddedSignal.connect(slot);
	}

	void removedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		boost::unique_lock<boost::mutex> lock(mRemovedComponentMutex);
		ServiceRemovedSignal.connect(slot);
	}

        /**
         * Relying on all running Servicediscovery within a process, this function
         * allows to search for visible services an all existing domains - search pattern 
         * allows to limit the search if required
         */
        static std::map<std::string, ServiceDescription> getVisibleServices(const SearchPattern& pattern = SearchPattern("")); 

        /**
         * Register callbacks on a specific ServiceDiscovery object running in the current process
         */
        static void registerCallbacks(const std::string& serviceName, const sigc::slot<void, ServiceEvent>& serviceAdded, const sigc::slot<void, ServiceEvent>& serviceRemoved);

private:

        // The internal modes for the service discovery, reflect the
        // usage as service publisher or listener only, i.e. 
        // to allow search one or more avahi domains, e.g. 
        // _test._tcp or similar
        enum Mode { NONE = 0, LISTEN_ONLY = 1, PUBLISH = 2};

	/**
	* Callback handler, when a service has been removed 
	*/
	void addedService(const RemoteService& service);

	/**
	* Callback handler, when a service has been removed
	*/
	void removedService(const RemoteService& service);

        /**
         * Callback handler, when service hase been updated 
         */
        void updatedService(const RemoteService& service);

        /**
         * Search for services -- without acquiring the lock on services
         */
	std::vector<ServiceDescription> _findServices(const SearchPattern& pattern) const;

        /**
         * Start service -- without acquiring the avahi poll loop lock
         * Use this function within the internal event handlers, since they are called from
         * the event loop, i.e. with acquired lock
         */
	void _start(const ServiceConfiguration& conf);

        /**
        * Stop the listening and publishing activities of the service discovery object
        * \param lock_client Set to true if the avahi client lock should be acquired
        * \param wait_until_unpublished Set to true if it needs to be waited for
        * the local service to be unpublished (this will not be necessary if the
        * avahi daemon restarts)
        */
	void _stop(bool lock_client, bool wait_until_unpublished = true);

        /**
         * Listen for services -- without acquiring the avahi poll loop lock
         * Use this function within the internal event handlers, since they are called from
         * the event loop, i.e. with acquired lock
         *
         */
	void _listenOn(const std::vector<std::string>& types);

        /**
         * Perform necessary steps to reestablish browser, resolver etc. after a avahi-daemon restart
         */
	void reconnect();

        /**
         * Cleanup all create avahi browser
         */
        void cleanupBrowsers();

	sigc::signal<void, ServiceEvent> ServiceAddedSignal;
	sigc::signal<void, ServiceEvent> ServiceRemovedSignal;

	mutable boost::mutex mAddedComponentMutex;
	mutable boost::mutex mRemovedComponentMutex;
	mutable boost::mutex mUpdatedComponentMutex;

	List<ServiceDescription> mServices;
	static boost::mutex mServicesMutex;

        // Flag whether service is published or not
        bool mPublished;

        // Seems to allow better thread safety and handling of the dbus if 
        // every browser is started with the same client object
        //
        // Map service type such as _myservice._tcp to a corresponding browser
	std::map<std::string, ServiceBrowser*> mBrowsers;

        // There can be only a single local service
	LocalService* mLocalService;
	// Configuration of the local service 
	ServiceConfiguration mLocalServiceConfiguration;

        // When in listing mode, the list of service types this instance
        // is listening to
        std::vector<std::string> mListenTypes;

        // Mode of the 
        Mode mMode;

        // Global list of service discovery instances
        static std::vector<ServiceDiscovery*> msServiceDiscoveries;

};

} // end namespace avahi
} // end namespace servicediscovery
#endif // _SERVICEDISCOVERY_CORE_H_
