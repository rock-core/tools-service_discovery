#include <boost/test/unit_test.hpp>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace servicediscovery::avahi;

BOOST_AUTO_TEST_SUITE(ServiceDiscoveryPatternSuite)

BOOST_AUTO_TEST_CASE(searchPatternTest)
{
    boost::regex test(".*");
    BOOST_REQUIRE( regex_match("",test) );

    ServiceDescription description;
    SearchPattern searchPattern0(".*",".*",".*",".*");

    BOOST_REQUIRE(searchPattern0.isMatching(description));

    description.setName("name");
    description.setDescription("label","x=0");
    description.setType("_test._tcp");

    SearchPattern searchPattern1("nam.*","lab.*","x=[0-9]",".*test.*");
    BOOST_REQUIRE(searchPattern1.isMatching(description));

    SearchPattern searchPattern2("unknown.*","lab.*","x=[0-9]",".*test.*");
    BOOST_REQUIRE(!searchPattern2.isMatching(description));
}

BOOST_AUTO_TEST_SUITE_END()

