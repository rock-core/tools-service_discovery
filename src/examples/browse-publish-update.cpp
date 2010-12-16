/**
 * @file
 * a example source file for browsing services and publishing and updating a service with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include "../afAvahiFramework.h"
 
 
void testUpdated (rock::communication::afRemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: UPDATED SERVICE: " << rms.getName() << std::endl;
}

void testAdded (rock::communication::afRemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getName() << std::endl;
	//connect a callback on service txt updates
	rms.serviceSignalConnect(sigc::ptr_fun(testUpdated));
}

 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
	rock::communication::afAvahiClient client;
	
	//create a service browser
	rock::communication::afServiceBrowser sbrowser(&client, "_rimres._tcp");

	//connect a callback to the service added signal. Method can also be a class member. Look at sigc++ api
	sbrowser.serviceAddedConnect(sigc::ptr_fun(testAdded));
	
	//publish a sample service to test the callbacks
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	rock::communication::afLocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, strlst);
	
	//run the main event loop (in this case in a different thread because default poll is afThreadPoll, so it program will continue normal execution)
	client.dispatch();
	
	sleep(5);
	
	//update the signal txt record
	((rock::communication::afThreadPoll*)client.getPoll())->lock();
	strlst.push_back("somethingelse=10");
	std::cout << "Updating string list\n";
	serv.updateStringList(strlst);
	((rock::communication::afThreadPoll*)client.getPoll())->unlock();
	
	sleep(5);
	
	//print services
	std::cout << "PRINTING SERVICES:\n";
	rock::communication::afList<rock::communication::afRemoteService> services = sbrowser.getServices();
	rock::communication::afList<rock::communication::afRemoteService>::iterator it;
	for (it = services.begin() ; it != services.end() ; it++) {
		std::cout << "     SERVICE: " << (*it).getName() << " " << (*it).getType() << " " << (*it).getInterface() << std::endl;
	}
	std::cout << "FINISHED PRINTING SERVICES.\n";

	sleep(5);
		
	//stop main loop. not really needed if nothing is done afterwards
	client.stop();	
	
}
