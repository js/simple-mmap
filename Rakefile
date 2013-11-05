#encoding: utf-8

require 'rubygems'
require 'rake'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "simple-mmap"
    gem.email = "pcnoordhuis@gmail.com"
    gem.homepage = "http://github.com/pietern/simple-mmap"
    gem.summary = "A simplistic interface for reading memory mapped files"
    gem.authors = ["Johan Sørensen", "Pieter Noordhuis"]

    gem.extensions = FileList["ext/**/extconf.rb"]
    gem.files = FileList['Rakefile', 'ext/**/*.{c,rb}', 'lib/**/*.rb', 'test/**/*.rb']
    gem.require_paths = ["ext", "lib"]

    gem.add_development_dependency "rake-compiler", "0.7.0"
  end
  Jeweler::GemcutterTasks.new
rescue LoadError
  puts "Jeweler (or a dependency) not available. Install it with: gem install jeweler"
end

gem 'rake-compiler', '>= 0.7.0'
require 'rake/extensiontask'
Rake::ExtensionTask.new('simple_mmap') do |ext|
  ext.name = "mapped_file"
  ext.lib_dir = File.join('lib', 'simple_mmap')
end

require 'rake/testtask'
Rake::TestTask.new(:test => :compile) do |test|
  test.libs << 'ext'
  test.verbose = true
end

task :default => :test
