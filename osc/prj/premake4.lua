--[[ A solution contains projects, and defines the available configurations

http://industriousone.com/premake/user-guide

example: http://opende.svn.sourceforge.net/viewvc/opende/trunk/build/premake4.lua?revision=1708&view=markup

http://bitbucket.org/anders/lightweight/src/tip/premake4.lua

]]
solution "oscframework"
	configurations { "Debug", "Release" }
	objdir "../obj"
 
-- oscframework library
project "oscframework"
	kind "StaticLib"
	language "C++"
	targetdir "../lib"
	files { "../src/oscframework/**.h", "../src/oscframework/**.cpp" }
	
	configuration "linux"
		buildoptions { "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs liblo`" }
	
	configuration "macosx"
		-- MacPorts
		includedirs { "/opt/local/include" }
		libdirs { "/opt/local/lib" }
		--links { "lo", "pthread" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" } 

-- test executable
project "oftests"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/tests/**.h", "../src/tests/**.cpp" }

	includedirs { "../src" }
	links { "oscframework" }

	configuration "linux"
		buildoptions { "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs liblo`" }

	configuration 'macosx'
		-- MacPorts
		includedirs { "/opt/local/include"}
		libdirs { "/opt/local/lib" }
		links { "lo", "pthread" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
