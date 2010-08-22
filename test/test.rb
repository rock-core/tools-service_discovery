require '../lib/ServiceDiscovery'
require 'test/unit'

class ServiceDiscoveryTest < Test::Unit::TestCase

	def test_ServiceDiscoveryStart
		name = "my-service"
		type = "_rimres._tcp"
		sd = ServiceDiscovery.new(name, type)
	end
	
	def test_ServiceDescription

		name = "my-service"
		sDescription = ServiceDescription.new(name)
	    	
		label = "my-label"	
		description = "my-service-description"
		sDescription.setDescription(label, description)
		retrievedDescription = sDescription.getDescription(label)
		assert_equal(description, retrievedDescription)

		labels = sDescription.getLabels
		labels.each do |l|
			assert_equal(label, l) 
		end
	end

	def test_ServiceDiscoveryInterfaces
		name = "my-service"
		type = "_rimres._tcp"
		sd = ServiceDiscovery.new(name, type)

		label = "my-label"
		description = "my-service-description"
		sd.setDescription(label, description)
		retrievedDescription = sd.getDescription(label)
		
		assert_equal(description, retrievedDescription)

		services = sd.findServices(name)
		emptyList = []
		assert_equal(emptyList, services)

		sd.start

		while true do
			puts "Running services: "
			services = sd.findServices("")
			services.each do |service|
				puts "#{service.getName}"
			end
			sleep 2
		end
		puts "--- End of List"
	end

end # Class end


