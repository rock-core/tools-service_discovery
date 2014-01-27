/**
 * @file
 * a example source file for publishing services and updating services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace std;
using namespace servicediscovery::avahi;
 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
        cout << "Publish a service named MyTestService" << endl;
	
	//publish a service on the network with arguments as: name, type, port, info, time to live in case of network/daemon failure
        ServiceConfiguration conf("MyTestService", "_mytype._tcp", 10000);
	conf.setDescription("service_year","1999");
	conf.setDescription("anotherinfo","infohere");
        conf.setTTL(300);

        ServiceDiscovery service;
        service.start(conf);
	
	sleep(10);

	conf.setDescription("somethingelse","10");
	cout << "Updating string list\n";
	service.update(conf);
	sleep(10);
}
