/**
 * @file
 * a example source file for browsing services and publishing and updating a service with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/service_discovery.h>
 
 
void testUpdated (servicediscovery::RemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: UPDATED SERVICE: " << rms.getName() << std::endl;
}

void testAdded (servicediscovery::RemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getName() << std::endl;
	//connect a callback on service txt updates
	rms.serviceSignalConnect(sigc::ptr_fun(testUpdated));
}

 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
	servicediscovery::Client client;
	
	//create a service browser
	servicediscovery::ServiceBrowser sbrowser(&client, "_rimres._tcp");

	//connect a callback to the service added signal. Method can also be a class member. Look at sigc++ api
	sbrowser.serviceAddedConnect(sigc::ptr_fun(testAdded));
	
	//publish a sample service to test the callbacks
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	servicediscovery::LocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, strlst);
	
	//run the main event loop (in this case in a different thread because default poll is ThreadPoll, so it program will continue normal execution)
	client.dispatch();
	
	sleep(5);
	
	//update the signal txt record
	client.lock();
	strlst.push_back("somethingelse=10");
	std::cout << "Updating string list\n";
	serv.updateStringList(strlst);
	client.unlock();
	
	sleep(5);
	
	//print services
	std::cout << "PRINTING SERVICES:\n";
	servicediscovery::List<servicediscovery::RemoteService> services = sbrowser.getServices();
	servicediscovery::List<servicediscovery::RemoteService>::iterator it;
	for (it = services.begin() ; it != services.end() ; it++) {
                servicediscovery::ServiceConfiguration config = it->getConfiguration();
		std::cout << "     SERVICE: " << config.getName() << " " << config.getType() << " " << config.getInterfaceIndex() << std::endl;
	}
	std::cout << "FINISHED PRINTING SERVICES.\n";

	sleep(5);
		
	//stop main loop. not really needed if nothing is done terwards
	client.stop();	
	
}
