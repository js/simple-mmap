# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'simple_mmap/version'

Gem::Specification.new do |spec|
  spec.name          = "simple-mmap"
  spec.version       = SimpleMmap::VERSION
  spec.authors       = ["Johan Sørensen", "Pieter Noordhuis", "TAKADA Daisuke"]
  spec.email         = ["pcnoordhuis@gmail.com", "shoulderpower@gmail.com"]
#  spec.description   = %q{TODO: Write a gem description}
  spec.description   = %q{A simplistic interface for reading memory mapped files}
  spec.summary       = %q{A simplistic interface for reading memory mapped files}
  spec.homepage      = "http://github.com/pietern/simple-mmap"
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["ext", "lib"]
  spec.extensions << 'ext/simple_mmap/extconf.rb'

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency 'rake-compiler', '>= 0.7.0'
end
