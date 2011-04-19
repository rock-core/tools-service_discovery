require 'servicediscovery'
require 'test/unit'

include Avahi

name = "test-service"
type = "_rimres._tcp"
sd = ServiceDiscovery.new

sd.set_description("IOR","your-ior")
sd.publish(name, type)
sd.listen_on(["_xgrid._tcp"])

alternative_sd = ServiceDiscovery.new
alternative_sd.publish("alternative-service", type)
alternative_sd.listen_on(["_alternative._tcp"])

puts "Listen: "
while true do
        puts "---------------------"
        puts "Service discovery 1: "
        services = sd.get_all_services
        services.each do |name|
                puts name 
        end
        puts "Service discovery alternative: "
        alt_services = alternative_sd.get_all_services
        alt_services.each do |name|
                puts name
        end
        sleep 1
end


