
#include <service-discovery/OrocosComponentService.h>

using namespace std;
using namespace dfki::communication;

int main (int argc, char const* argv[])
{
	
	list<string> ls;
	ls.push_back("DSFDSF");
	ls.push_back("23");
	ls.push_back("IORSIZE=2");
	ls.push_back("IOR1=prvdel");
	ls.push_back("IOR2=vtordel");
	
	afAvahiClient client;
	
	afServiceBrowser browser(&client, "_rimres._tcp");
	
	AvahiAddress addr;
	
	OrocosComponentRemoteService rserv(&browser, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "SDF", "_rimres._tcp", "", ls, 10000, "MYSHO", addr, NULL);
	
	cout << rserv.getIOR() << endl;
	
	OrocosComponentLocalService lserv(&client, "MyOrocosService", "_rimres._tcp", 10000, 
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890") +
			string("12345678901234567890123456789012345678901234567890"),
		rserv.getStringList());
		
	client.dispatch();
	
	sleep(10);
	
	return 0;
}
