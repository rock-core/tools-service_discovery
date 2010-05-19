
#define AF_LOG_CONFIG_FILE "af-log.properties"

#include <service-discovery/afAvahiFramework.h>


namespace dc = dfki::communication;

int main (int argc, char const* argv[])
{
	log4cxx::PropertyConfigurator::configure("af-log.properties");
	dc::afAvahiClient client;
	dc::afServiceBrowser sbrowser(&client, "_rimres._tcp");
	dc::afLocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, std::list<std::string>());
	client.dispatch();
	sleep(2);
	client.stop();
	return 0;
}
