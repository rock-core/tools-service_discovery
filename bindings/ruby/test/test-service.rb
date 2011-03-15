require 'servicediscovery_ruby'
require 'test/unit'

name = "test-service"
type = "_test._tcp"
sd = ServiceDiscovery.new(name, type)
sd.setDescription("IOR","your-ior")
sd.start
while true do
	sleep 0.5
end


