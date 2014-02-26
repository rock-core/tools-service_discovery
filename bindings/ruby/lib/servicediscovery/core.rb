require 'servicediscovery_ruby'

module Avahi
    class ServiceDiscovery
        # Search for services using pattern match on
        # servicename, label and text within the description data, 
        # or the type (such such as _test._tcp)
        # by default searches for all available services
        # @param [ String ] service name pattern
        # @param [ String ] label pattern to match label of a description entry
        # @param [ String ] txt pattern to match to content of a description
        # @param [ String ] service type pattern
        # @return [Array<Avahi::ServiceDescription>]
        def find_services(servicename = ".*", label = ".*", txt = ".*", type = ".*")
            services = do_find_services(servicename, label, txt, type)
            services
        end
    end

end
