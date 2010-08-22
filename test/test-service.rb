require '../lib/ServiceDiscovery'
require 'test/unit'

name = "test-service"
type = "_rimres._tcp"
sd = ServiceDiscovery.new(name, type)
sd.start
while true do
	sleep 0.5
end


