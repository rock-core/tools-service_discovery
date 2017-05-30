#include <boost/test/unit_test.hpp>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace servicediscovery::avahi;

struct ConfigurationSetup
{
};

BOOST_FIXTURE_TEST_SUITE(ServiceDiscoverySuite, ConfigurationSetup)

BOOST_AUTO_TEST_CASE(configurationTest)
{
    ServiceDescription description("test-service");
    for(int i = 0; i < 10; ++i)
    {
        char buffer[100];
        sprintf(buffer, "label-%d", i);
        std::string label(buffer);

        sprintf(buffer, "value-%d", i);
        std::string value(buffer);

        description.setDescription(label, value);
    }

    for(int i = 0; i< 10; ++i)
    {
        char buffer[100];
        sprintf(buffer, "label-%d", i);
        std::string label(buffer);

        sprintf(buffer, "value-%d", i);
        std::string expectedValue(buffer);

        std::string currentValue = description.getDescription(label);

        BOOST_CHECK_MESSAGE(currentValue == expectedValue, "Expected value ok");
    }

}

BOOST_AUTO_TEST_SUITE_END()
