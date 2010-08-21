require 'rake'
require 'rake/gempackagetask'

### begin gem task ###
# For generating a gem into: pkg/ just call
# $rake gem
# afterwards you can install it via 
# $sudo gem install pkg/<name>-<version>.gem
#
# further documentation at: http://www.rubygems.org/read/book/4
spec = Gem::Specification.new do |s|
  s.name 	 = "servicediscovery"
  s.summary      = "Avahi based servicediscovery"
  s.description  = File.read(File.join(File.dirname(__FILE__), 'README'))
  s.requirements = [ 'service-discovery' ]
  s.version      = "0.0.1"
  s.author       = "Thomas Roehr"
  s.email        = "thomas.roehr@dfki.de"
  s.homepage     = "http://www.dfki.de/robotic"
  s.platform     = Gem::Platform::RUBY
  s.required_ruby_version = '>=1.8.7'
  s.files        = Dir['**/**/**']
  s.executables  = ['']
  s.test_files   = Dir["test/test*.rb"]
  s.has_rdoc     = false
end

Rake::GemPackageTask.new(spec).define do |pkg|
	pkg.need_zip = true
	pkg.need_tar = true
end
### end gem task ###

### General setup task for local compilation
# since it is the default (see below) just call
# $rake 
#
RUBY = RbConfig::CONFIG['RUBY_INSTALL_NAME']

desc "build ruby ServiceDiscovery extension"
namespace :setup do
	desc "builds ServiceDiscovery C++ extension"
	task :ext do
		builddir = File.join('ext', 'build')
		prefix   = File.join(Dir.pwd, 'ext')
	
		FileUtils.mkdir_p builddir
		Dir.chdir(builddir) do
			if !system("#{RUBY} ../extconf.rb") || !system("make")
				throw "unable to build the extension"
			end
		end
		FileUtils.ln_sf "../ext/build/ServiceDiscovery.so", "lib"
	end
end

task :setup => [ "setup:ext" ]

desc "remove by-products of setup"
task :clean do
	FileUtils.rm_rf "pkg/"
	if File.exists?("ext/build")
		FileUtils.rm_rf("ext/build")
	end

	if File.exists?("lib/ServiceDiscovery.so")
		FileUtils.rm "lib/ServiceDiscovery.so"
	end
end

desc "build ruby package ServiceDiscovery"
task :fipamessage => [ "setup" ] do 
end

#Set the default task for the Rakefile to be 'setup'
task :default => :fipamessage



  
