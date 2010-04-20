--[[ A solution contains projects, and defines the available configurations

http://industriousone.com/premake/user-guide

example: http://opende.svn.sourceforge.net/viewvc/opende/trunk/build/premake4.lua?revision=1708&view=markup

http://bitbucket.org/anders/lightweight/src/tip/premake4.lua

]]
solution "xmlframework"
	configurations { "Debug", "Release" }
	objdir "../obj"
 
-- oscframework library
project "xmlframework"
	kind "StaticLib"
	language "C++"
	targetdir "../lib"
	files { "../src/xmlframework/**.h", "../src/xmlframework/**.cpp" }

	configuration "macosx"
		buildoptions { "-fvisibility=default" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" } 

-- test executable
project "xmltests"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/tests/**.h", "../src/tests/**.cpp" }

	includedirs { "../src/xmlframework" }
	links { "xmlframework" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
