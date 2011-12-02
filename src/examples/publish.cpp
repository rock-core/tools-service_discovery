/**
 * @file
 * a example source file for publishing services and updating services with the avahi c++ framework
 */
 
#include <stdlib.h>
#include <iostream>
#include <service_discovery/service_discovery.h>

using namespace std;
 
int main(int argc, char** argv)
{
	
	//create a client with default constructor with threaded poll
	servicediscovery::Client* client = servicediscovery::Client::getInstance();
        cout << "Publish a service named MyTestService" << endl;
	
	std::list<std::string> strlst;
	strlst.push_back("service_year=1999");
	strlst.push_back("anotherinfo=infohere");
	//publish a service on the network with arguments as: name, type, port, info, time to live in case of network/daemon failure
	servicediscovery::LocalService serv(client, "MyTestService", "_mytype._tcp", 10000, strlst, 300);
	
	sleep(10);
	
	client->lock();
	strlst.push_back("somethingelse=10");
	cout << "Updating string list\n";
	serv.updateStringList(strlst);
	client->unlock();

	sleep(10);

	
}
