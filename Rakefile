require "bundler/gem_tasks"

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
