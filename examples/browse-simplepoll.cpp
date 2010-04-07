/**
 * a example source file for browsing services using simple poll
 */
 
#include <csignal>
#include <stdlib.h>
#include <iostream>
#include <service-discovery/afAvahiFramework.h>
 
//create a client with simple poll
afSimplePoll simplepoll;
afAvahiClient client(&simplepoll, (AvahiClientFlags) 0);
 
void handleSIGINT(int sig) {
	std::cout << ".CAUGHT SIG " << sig << std::endl;
	client.stop();
} 
 
void testAdded (afRemoteService* rms) {
	std::cout << " -=- TESTING SIGNAL: ADDED SERVICE: " << rms->getName() << std::endl;
}

 
int main(int argc, char** argv)
{
	
	signal(SIGTERM, handleSIGINT);
	signal(SIGINT, handleSIGINT);
	
	//create a service browser
	afServiceBrowser sbrowser(&client, "_rimres._tcp");

	//connect a callback to the service added signal. Method can also be a class member. Look at sigc++ api
	sbrowser.afServiceAdded.connect(sigc::ptr_fun(testAdded));
	
	//publish a sample service to test the callbacks
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	afLocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, strlst);
	
	//run the main event loop
	client.dispatch();
	
}
