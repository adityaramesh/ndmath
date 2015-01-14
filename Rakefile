require 'rake'
require 'rake/clean'

cxx    = ENV['CXX']
boost  = ENV['BOOST_INCLUDE_PATH']
ccbase = ENV['CCBASE_INCLUDE_PATH']

langflags  = "-std=c++1y"
wflags     = "-Wall -Wextra -pedantic -Wno-missing-field-initializers"
archflags  = "-march=native"
incflags   = "-I include -isystem #{boost} -isystem #{ccbase}"
ldflags    = ""

debug_optflags = "-O2"
if cxx.include? "clang"
	release_optflags = "-Ofast -fno-fast-math -flto -fno-unroll-loops"
elsif cxx.include? "g++"
	release_optflags = "-Ofast -fno-fast-math -flto"
end

base_cxxflags = "#{langflags} #{wflags} #{archflags} #{incflags}"
debug_cxxflags = "#{base_cxxflags} #{debug_optflags}"
release_cxxflags = "#{base_cxxflags} #{release_optflags}"

dirs  = ["out"]
tests = ["out/range_test.run"] #FileList["source/test/*"].map{|f| f.sub("source/test", "out").ext("run")}

multitask :default, [:mode] => dirs + tests
multitask :tests, [:mode] => dirs + tests

task :syntax, [:header] do |t, args|
	src = "include/ndmath/#{args[:header]}.hpp"
	sh "#{cxx} #{base_cxxflags} #{src}"
end

task :ppdump, [:header] do |t, args|
	src = "include/ndmath/#{args[:header]}.hpp"
	sh "#{cxx} -E #{base_cxxflags} #{src}"
end

task :asmdump, [:test] do |t, args|
	src = "source/test/#{args[:test]}.cpp"
	sh "#{cxx} -S #{release_cxxflags} #{src} #{ldflags}"
end

dirs.each do |d|
	directory d
end

tests.each do |f|
	src = f.sub("out", "source/test").ext("cpp")
	file f, [:mode] => [src] + dirs do |t, args|
		args.with_defaults(:mode => :release)
		if args[:mode] == "debug"
			sh "#{cxx} #{debug_cxxflags} -o #{f} #{src} #{ldflags}"
		elsif args[:mode] == "release"
			sh "#{cxx} #{release_cxxflags} -o #{f} #{src} #{ldflags}"
		end
	end
end

task "check" => dirs + tests do
	tests.each do |f|
		r = `./#{f} -v medium`
		if r.include? "Failure"
			puts "The test \"#{f}\" failed! Please submit a bug report."
			puts r
		end
	end
end

task :clobber => dirs do
	FileList["out/*.run"].each{|f| File.delete(f)}
end
