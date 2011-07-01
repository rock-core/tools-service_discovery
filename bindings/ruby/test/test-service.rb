require 'servicediscovery'
require 'test/unit'

include Avahi

name = "test-service"
type = "_rimres._tcp"
sd = ServiceDiscovery.new
ior = "IOR:010000001f00000049444c3a5254542f636f7262612f435461736b436f6e746578743a312e30000001000000000000006800000001010200100000003139322e3136382e3130312e31313300f38d00000e000000fea60d0e4e00002d93000000000000000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100"

sd.set_description("IOR",ior)
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


