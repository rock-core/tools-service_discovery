/**
 * @file
 * a example source file for publishing services and updating services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include "../afAvahiFramework.h"
#include <iostream>

using namespace std;
 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
	dfki::communication::afAvahiClient client;
	
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	strlst.push_back("anotherinfo=infohere");
	//publish a service on the network with arguments as: name, type, port, info, time to live in case of network/daemon failure
	dfki::communication::afLocalService serv(&client, "MyTestService", "_mytype._tcp", 10000, strlst, 300);
	
	//run the main event loop (in this case in a different thread because default poll is afThreadPoll, so it program will continue normal execution)
	client.dispatch();
	
	sleep(10);
	
	((dfki::communication::afThreadPoll*) client.getPoll())->lock();
	strlst.push_back("somethingelse=10");
	cout << "Updating string list\n";
	serv.updateStringList(strlst);
	((dfki::communication::afThreadPoll*) client.getPoll())->unlock();

	sleep(10);

	//stop main loop. not really needed if nothing is done afterwards
	client.stop();	
	
}
