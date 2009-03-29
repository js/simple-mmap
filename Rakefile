# encoding: utf-8

require 'rubygems'
require 'hoe'
require './lib/simple_mmap/version.rb'
require "rake/clean"

Hoe.new('simple_mmap', SimpleMmap::VERSION) do |p|
  p.rubyforge_name = 'simple-mmap'
  p.developer('Johan Sørensen', 'johan@johansorensen.com')
  p.spec_extras = {
    "extensions" => ["ext/extconf.rb"]
  }
end

DLEXT = Config::CONFIG['DLEXT']

file 'ext/Makefile' => FileList['ext/{*.c,*.h,*.rb}'] do
  chdir('ext') { ruby 'extconf.rb' }
end
CLEAN.include 'ext/Makefile', 'ext/mkmf.log'

file "ext/mapped_file.#{DLEXT}" => FileList['ext/Makefile', 'ext/*.{c,h,rb}'] do |f|
  sh 'cd ext && make'
end
CLEAN.include 'ext/*.{o,bundle,so,dll}'

file "lib/simple_mmap/mapped_file.#{DLEXT}" => "ext/mapped_file.#{DLEXT}" do |f|
  cp f.prerequisites, "lib/simple_mmap/", :preserve => true
end
#CLEAN.include "lib/simple_mmap/mapped_file.#{DLEXT}"

desc 'Build the mapped_file extension'
task :build => "lib/simple_mmap/mapped_file.#{DLEXT}"

task :test => [:build]

task :default => :test
