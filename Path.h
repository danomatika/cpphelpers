/*==============================================================================

	Path.h

	Copyright (C) 2015 Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#pragma once

#include <string>
#include <sstream>
#include <sys/stat.h>

#if !defined( __WIN32__ ) && !defined( _WIN32 )
	#include <unistd.h>
#endif

/// \class Path
/// \brief cross-platform path string functions
class Path {

	public:

		/// returns current directory
		static string currentDir() {
			std::string path;
			char currDir[PATH_MAX];
			#if defined( __WIN32__ ) || defined( _WIN32 )
				path = _getcwd(currDir, PATH_MAX);
				path.replace(path.begin(), path.end(), "/", "\\"); // fix any unixy paths...
			#else // Mac / Linux
				path = getcwd(currDir, PATH_MAX);
			#endif
			return path;
		}

		/// returns true if a path exists
		static bool exists(const std::string & path) {
			return access(path.c_str(), F_OK) == 0;
		}

		/// returns true if a path exists and is readable
		static bool isReadable(const std::string & path) {
			return access(path.c_str(), R_OK) == 0;
		}

		/// returns true if a path exists and is writable
		static bool isWritable(const std::string & path) {
			return access(path.c_str(), W_OK) == 0;
		}

		/// returns true if a path exists and is executable
		static bool isExecutable(const std::string & path) {
			return access(path.c_str(), X_OK) == 0;
		}

		/// returns true if path is absolute, false if relative
		static bool isAbsolute(std::string path) {
			if(path.length() != 0 &&
				(path[0] == separator || path.substr(1, 1) == ":")) {
				return true;
			}
			return false;
		}

		/// convert a given relative path to an absolute path using the
		/// current dir, passes through existing absolute paths
		/// adapted from openframeworks ofToDataPath():
		/// https://github.com/openframeworks/openFrameworks/blob/master/libs/openFrameworks/utils/ofUtils.cpp
		static std::string absolutePath(std::string path) {
			if(isAbsolute(path)) {
				return path; // pass through
			}
			return append(currentDir(), path);
		}

		/// append two paths
		static std::string append(std::string path1, std::string path2) {
			return path1 + separator + path2;
		}

		/// last path component in a path
		static std::string lastComponent(std::string path) {
			size_t pos = path.rfind(separator);
			if(pos == std::string::npos) {
				pos = 0;
			}
			return path.substr(pos, path.size()-pos);
		}

		/// path minus the last component
		static std::string withoutLastComponent(std::string path) {
			size_t pos = path.rfind(separator);
			if(pos == std::string::npos) {
				pos = path.size();
			}
			return path.substr(0, pos);
		}

		/// split the path into it's components
		static std::vector<string> split(std::string path) {
			std::vector<string> components;
			std::istringstream line(path);
			std::string token;
			while(std::getline(line, token, separator)) {
				components.push_back(token);
			}
			return components;
		}

		/// join path components into a string
		static std::string join(std::vector<std::string> &components) {
			std::string path = "";
			for(unsigned int i = 0; i < components.size(); ++i) {
				path = path + separator + components[i];
			}
			return path;
		}

		/// platform path separator
		#if defined( __WIN32__ ) || defined( _WIN32 )
			static const char separator = '\\';
		#else // Mac / Linux
			static const char separator = '/';
		#endif
};
