
#define AF_LOG_CONFIG_FILE "af-log.properties"

#include "../afAvahiFramework.h"


namespace dc = rock::communication;

int main (int argc, char const* argv[])
{
	dc::afConfigurator::getInstance()->configureLoggingProperties("af-log.properties");
	dc::afAvahiClient client;
	dc::afServiceBrowser sbrowser(&client, "_rimres._tcp");
	dc::afLocalService serv(&client, "MyTestService", "_rimres._tcp", 10000, std::list<std::string>());
	client.dispatch();
	sleep(4);
	client.stop();
	return 0;
}
