--[[

the premake4 build script for this project

generates Makefiles, VS projects, and Xcode projects

for more information on Premake: http://industriousone.com/premake

reference: http://industriousone.com/premake/user-guide

examples:
 - http://opende.svn.sourceforge.net/viewvc/opende/trunk/build/premake4.lua?revision=1708&view=markup
 - http://bitbucket.org/anders/lightweight/src/tip/premake4.lua
 
]]
solution "projectName"
	configurations { "Debug", "Release" }
	objdir "../obj"
 
-- convenience library
project "projectLib"
	kind "StaticLib"
	language "C++"
	targetdir "../lib"
	files { "../src/lib/**.h", "../src/lib/**.cpp" }
	
	includedirs { "../src"}
	libdirs {  }
	
	configuration "linux"
		defines { "LINUX" }
		buildoptions { "`pkg-config --cflags sdl`" }
		linkoptions { "`pkg-config --libs sdl`" }
	
	configuration "macosx"
		defines { "MACOSX" }
		-- get rid of visibilty warnings
		buildoptions { "-fvisibility-inlines-hidden" }
		-- MacPorts
		includedirs { "/opt/local/include" }
		libdirs { "/opt/local/lib" }

	configuration { "Windows" }
		defines { "WIN32" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" } 

-- program
project "projectProgram"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/**.h", "../src/**.cpp" }
	
	includedirs { "../src", "../src/lib" }

	libdirs { }
	links { "projectLib" }

	configuration "linux"
		defines { "LINUX" }
		buildoptions { "`pkg-config --cflags sdl`" }
		linkoptions { "`pkg-config --libs sdl`" }

	configuration "macosx"
		defines { "MACOSX" }
		-- MacPorts
		includedirs { "/opt/local/include"}
		libdirs { "/opt/local/lib" }
		links { "lo", "pthread", "SDLmain", "SDL" }
		linkoptions { "-Wl,-framework,Cocoa", "-Wl,-framework,OpenGL",
					  "-Wl,-framework,ApplicationServices",
					  "-Wl,-framework,Carbon", "-Wl,-framework,AudioToolbox",
					  "-Wl,-framework,AudioUnit", "-Wl,-framework,IOKit" }

	configuration { "Windows" }
		defines { "WIN32" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
