/**
 * @file
 * a example source file for browsing services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "../ServiceDiscovery.h"

using namespace dfki::communication;

/*void testAdded (ServiceEvent rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getName() << std::endl;
}

void testRemoved (dfki::communication::afRemoteService rms) {
	std::cout << " -=- TESTING SIGNAL: REMOVED SERVICE: " << rms.getName() << std::endl;
}
*/
void callbackFunction (ServiceEvent rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms.getServiceDescription().getName() << std::endl;
}

 
int main(int argc, char** argv)
{

	std::string someServiceData = "IOR:010000001e00000049444c3a5254542f436f7262612f436f6e74726f6c5462736b3a312e30000000010000000000000068000000010102000f0000003139322e3136382e3130312e3638000026e300000e000000fe35313c4c0000278c000000000000000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100";
	ServiceDiscovery service;
	ServiceConfiguration conf("ModuleB", "_module._tcp");
	conf.setDescription("IOR", someServiceData);
	service.addedComponentConnect(sigc::ptr_fun(callbackFunction));
	service.start(conf);
	sleep(150);

	return 0;
/*	//create a client with default constructor with threaded poll
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
*/	
}
