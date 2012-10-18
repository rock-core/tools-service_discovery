require 'servicediscovery_ruby'

module Avahi
    class ServiceDiscovery
        # Search for service using pattern match on 
        # servicename, label and text within the description data, 
        # or the type (such such as _test._tcp)
        def find_services(servicename, label = ".*", txt = ".*", type = ".*")
            services = do_find_services(servicename, label, txt, type)
            services
        end
    end

end
