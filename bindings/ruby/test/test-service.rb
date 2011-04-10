require 'servicediscovery_ruby'
require 'test/unit'

include Avahi

name = "test-service"
type = "_xgrid._tcp"
sd = ServiceDiscovery.new(name, type)
sd.set_description("IOR","your-ior")
#sd.start
sd.listen_on(["_xgrid._tcp"])
while true do
        puts "Search"
        services = sd.find_services("rlb-xserve")

        services.each do |desc|
                puts desc.get_name
        end
	sleep 5
end


