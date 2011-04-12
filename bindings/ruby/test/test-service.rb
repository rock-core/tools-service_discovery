require 'servicediscovery_ruby'
require 'test/unit'

include Avahi

name = "test-service"
type = "_rimres._tcp"
sd = ServiceDiscovery.new
sd.set_description("IOR","your-ior")
sd.publish(name, type)
sd.listen_on(["_xgrid._tcp"])
puts "Listen: "
while true do
        services = sd.get_all_services

        services.each do |name|
                puts name 
        end
        sleep 1
end


