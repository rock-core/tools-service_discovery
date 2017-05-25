#include <stdio.h>
#include <service_discovery/ServiceDiscovery.hpp>

using namespace service_discovery;

int main(int argc, char** argv)
{
    printf("Hello World\n");

    ServiceConfiguration sc("discover", "_rimres._tcp");
    sc.setDescription("IOR", "123");

    ServiceDiscovery discover;
    discover.start(sc);

    ServiceDiscovery::SearchPattern pattern("test-service");

    while(true) {
        printf("---\n");

        std::vector<ServiceDescription> desc = discover.findServices(pattern);

        if( desc.size() > 0 ) {
            printf("found '%s'( ", desc.front().getName().c_str());

            std::vector<std::string> labels = desc.front().getLabels();

            for(unsigned int i = 0; i < labels.size(); i++) {
                printf("%s: %s ", labels[i].c_str(), desc.front().getDescription(labels[i]).c_str());
            }

            printf(")\n");
        }

        sleep(1);
    }

    return 0;
}
