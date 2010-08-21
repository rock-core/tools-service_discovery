#Loads mkmf which is used to make makefiles for Ruby extensions
require 'rubygems'
#workaround for require 'mkmf-rice'
gem 'rice', '~> 1.3.2'

require 'mkmf-rice'
extension_name = 'ServiceDiscovery'
#Set all dependencies here
dependencies = [ 'service-discovery' ]


# check if the dependencies are all available and retrieve 'em via pkg_config
dependencies.each do |pkg|
	pkg_config(pkg)
end

# Create the makefile
create_makefile(extension_name)
