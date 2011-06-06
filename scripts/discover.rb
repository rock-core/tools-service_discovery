require "servicediscovery"

include Avahi

name = "discover-service"
type = "_rimres._tcp"
service = ServiceDiscovery.new

service.set_description("IOR", "ior")
service.publish(name, type)

puts "Listen: "
while true do
    services = service.get_all_services
    services.each do |name|
        desc = service.find_services(name)

        desc.each do |item|
            print "#{name}: "

            item.get_labels.each do |label|
                print "#{label}=#{item.get_description(label)} "
            end

            puts ""
        end
    end
    
    sleep 1
end
