require "servicediscovery"

include Avahi

name = "test-service"
type = "_rimres._tcp"
service = ServiceDiscovery.new

service.set_description("IOR", "your-ior")
service.set_description("x", "10")
service.publish(name, type)

sleep 10

service.set_description("IOR", "test")
service.update

puts "UPDATED"

begin
while true do
    sleep 1
end
rescue Interrupt => e
    puts "SHUTDOWN"
end

