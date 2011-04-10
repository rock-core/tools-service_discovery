/*
 * remote_service.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include <service_discovery/remote_service.h>
#include <iostream>

namespace servicediscovery { 

static LoggingWrapper logger("RemoteService");

RemoteService::RemoteService(
			ServiceBrowser *browser,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain,
			std::list<std::string> list,
			uint16_t port,
			std::string host_name,
			AvahiAddress address,
			AvahiServiceResolver *sr,
			sigc::signal<void,
		RemoteService> *RemoteServiceSignal

	) : Service(browser->getClient(), interf, prot, name, type, domain, port, list) {
	this->host_name = host_name;
	this->address = address;
	this->sr = sr;
	this->RemoteServiceSignal = RemoteServiceSignal;

	if (sem_init(&RMS_sem,0,1) == -1) {
		logger.log(FATAL, "Semaphore initialization failed");
		throw 1;
	}
	
}

RemoteService::~RemoteService() {

	if (sem_destroy(&RMS_sem) == -1) {
		logger.log(WARN, "Semaphore destruction failed");
	}
}


bool RemoteService::operator ==(RemoteService serv) {
	bool upres = (Service) (*this) == (Service) serv;
	if (!upres)
		return false;
	if (host_name.compare(serv.getHostName()) != 0)
		return false;
	if (getAddressString().compare(serv.getAddressString()) != 0)
		return false;
	return true;
}

} // end namespace servicediscovery

