require 'servicediscovery'
require 'test/unit'

include Avahi

class ServiceDiscoveryTest < Test::Unit::TestCase

	def test_ServiceDiscoveryStart
		name = "my-service"
		type = "_rimres._tcp"
		sd = ServiceDiscovery.new
		sd.listen_on(["_test._tcp"])
	end
	
	def test_ServiceDescription

		name = "my-service"
		sDescription = ServiceDescription.new(name)
	    	
		label = "my-label"	
		description = "my-service-description"
		sDescription.set_description(label, description)
		retrievedDescription = sDescription.get_description(label)
		assert_equal(description, retrievedDescription)

                label = "IOR"
                description = "IOR:010000001f00000049444c3a5254542f636f7262612f435461736b436f6e746578743a312e30000001000000000000006800000001010200100000003139322e3136382e3130312e31313300f38d00000e000000fea60d0e4e00002d93000000000000000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100"
		sDescription.set_description(label, description)
		retrievedDescription = sDescription.get_description(label)
		assert_equal(description, retrievedDescription)

		labels = sDescription.get_labels
                assert_equal(0, labels.index("my-label"))
                assert_equal(1, labels.index("IOR"))
	end

	def test_ServiceDiscoveryInterfaces
		name = "my-service"
		type = "_rimres._tcp"
		sd = ServiceDiscovery.new


		label = "my-label"
		description = "my-service-description"
		sd.set_description(label, description)
		retrievedDescription = sd.get_description(label)
		
		assert_equal(description, retrievedDescription)

		services = sd.find_services(name)
		emptyList = []
		assert_equal(emptyList, services)

		sd.listen_on(["_test._tcp"])

		for i in 1..10 do
			puts "Running services: "
			services = sd.find_services("")
			services.each do |service|
				content = service.get_description "test-label"
				puts "#{service.getName}"
				puts "label content: #{content}"
			end

			puts "--- End of List"
			sleep 1
		end
	end

end # Class end


