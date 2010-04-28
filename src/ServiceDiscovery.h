#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include <vector>
#include "afAvahiClient.h"
#include "afServiceBrowser.h"
#include "OrocosComponentService.h"

namespace dfki { namespace communication {


enum SDException {
	already_started,
	not_configured
};

/**
 * @brief
 * A wrapper class for the framework
 */
class ServiceDiscovery : public sigc::trackable
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

	struct Configuration // : default configuration here
	{
	
	
		Configuration(std::string IOR, std::string name, std::string avahi_type) : avahi_port(12000), ttl(0) {
			this->IOR = IOR;
			this->name = name;
			this->avahi_type = avahi_type;
		}
		Configuration(std::string IOR, std::string name, std::string avahi_type, uint16_t avahi_port, uint32_t ttl) {
			this->IOR = IOR;
			this->name = name;
			this->avahi_type = avahi_type;
			this->avahi_port = avahi_port;
			this->ttl = ttl;
		}
		
		std::string IOR;
		std::string name;

		std::list<std::string> stringlist;
		
		uint32_t ttl;
		
		//should these be constants?
		std::string avahi_type;
		uint16_t avahi_port; 

	};

	void configure(const struct Configuration& configuration);

	void start();

	void stop();

	/**
	 * name searches in service name and txt for txt records. both are "OR"-ed
	 */
	struct SearchPattern 
	{
		SearchPattern(std::string name) : txt("") {
			this->name = name;
		}

		SearchPattern(std::string name, std::string txt) {
			this->name = name;
		}
		std::string name;
		std::string txt;
	};
		
	std::vector<OrocosComponentRemoteService> findServices(SearchPattern pattern);

	sigc::signal<void, OrocosComponentRemoteService> OrocosComponentAddedSignal;
	sigc::signal<void, OrocosComponentRemoteService> OrocosComponentRemovedSignal;

private:

	void addedService(afRemoteService serv);
	void removedService(afRemoteService serv);

	afList<OrocosComponentRemoteService> services;

	bool started;

	afAvahiClient* client;
	afServiceBrowser* browser;
	OrocosComponentLocalService* localserv;

	bool configured;
	struct Configuration conf;

};

}}

#endif
