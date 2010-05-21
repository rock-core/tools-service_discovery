/**
 * @file
 * a example source file for browsing services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include "../afAvahiFramework.h"
 

void testAdded (dfki::communication::afRemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getName() << std::endl;
}

void testRemoved (dfki::communication::afRemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: REMOVED SERVICE: " << rms.getName() << std::endl;
}

 
int main(int argc, char** argv)
{
	//create a client with default constructor with threaded poll
	dfki::communication::afAvahiClient client;
	
	//create a service browser
	dfki::communication::afServiceBrowser sbrowser(&client, "_rimres._tcp");

	//connect a callback to the service added signal. Method can also be a class member. Look at sigc++ api
	sbrowser.serviceAddedConnect(sigc::ptr_fun(testAdded));
	
	//connect a callback to the service removed signal
	sbrowser.serviceRemovedConnect(sigc::ptr_fun(testRemoved));

	//publish a sample service to test the callbacks
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	dfki::communication::afLocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, strlst);
	
	//run the main event loop (in this case in a different thread because default poll is afThreadPoll, so it program will continue normal execution)
	client.dispatch();
	
	sleep(3);
	
	//print services
	std::cout << "PRINTING SERVICES:\n";
	dfki::communication::afList<dfki::communication::afRemoteService> services = sbrowser.getServices();
	dfki::communication::afList<dfki::communication::afRemoteService>::iterator it;
	for (it = services.begin() ; it != services.end() ; it++) {
		std::cout << "     SERVICE: " << (*it).getName() << " " << (*it).getType() << " " << (*it).getInterface() << std::endl;
	}
	std::cout << "FINISHED PRINTING SERVICES.\n";
	
	sleep(2);
	
	//unpublish the service and check for the signal. first must lock the object to prevent concurrency issues
	((dfki::communication::afThreadPoll*)client.getPoll())->lock();
	serv.unpublish();
	((dfki::communication::afThreadPoll*)client.getPoll())->unlock();
	
	sleep(2);
	
	//stop main loop. not really needed if nothing is done afterwards
	client.stop();	
	
}
