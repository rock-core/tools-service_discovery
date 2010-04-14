/**
 * a example source file for publishing services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <service-discovery/afAvahiFramework.h>
 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
	afAvahiClient client;
	
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	afLocalService serv(&client, "MyTestService", "_mysubtype._mytype._tcp", 10000, strlst);
	
	//run the main event loop (in this case in a different thread because default poll is afThreadPoll, so it program will continue normal execution)
	client.dispatch();
	
	sleep(10);
	
	//stop main loop. not really needed if nothing is done afterwards
	client.stop();	
	
}
