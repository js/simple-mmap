require 'rubygems'
require 'rake'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "simple-mmap"
    gem.email = "pcnoordhuis@gmail.com"
    gem.homepage = "http://github.com/pietern/simple-mmap"
    gem.summary = "Read-only wrapper for mmap"
    gem.authors = ["Johan Sørensen", "Pieter Noordhuis"]

    gem.extensions = ["ext/extconf.rb"]
    gem.files = FileList['ext/Makefile', 'ext/*.{c,rb}', 'lib/**/*.rb', 'test/**/*.rb']
    gem.require_paths = ["ext", "lib"]
  end
  Jeweler::GemcutterTasks.new
rescue LoadError
  puts "Jeweler (or a dependency) not available. Install it with: gem install jeweler"
end

DLEXT = Config::CONFIG['DLEXT']
file 'ext/Makefile' => FileList['ext/{*.c,*.h,*.rb}'] do
  chdir('ext') { ruby 'extconf.rb' }
end

file "ext/mapped_file.#{DLEXT}" => FileList['ext/Makefile', 'ext/*.{c,h,rb}'] do |f|
  sh 'cd ext && make'
end

namespace :ext do
  task :clean do
    %W[
      ext/Makefile
      ext/mapped_file.#{DLEXT}
    ].each do |file|
      sh "rm -f #{file}"
    end
  end

  desc 'Build the mapped_file extension'
  task :build => [:clean, "ext/mapped_file.#{DLEXT}"]
end

require 'rake/testtask'
Rake::TestTask.new(:test => "ext:build") do |test|
  test.libs << 'ext'
  test.verbose = true
end

task :default => :test
