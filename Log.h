/*==============================================================================

	Log.h

	Copyright (C) 2010, 2024 Dan Wilcox <danomatika@gmail.com>

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

#include <iostream>
#include <sstream>

// convenience defines
#define LOG         Log(Log::LEVEL_NORMAL)
#define LOG_DEBUG   Log(Log::LEVEL_DEBUG)
#define LOG_VERBOSE Log(Log::LEVEL_VERBOSE)
#define LOG_WARN    Log(Log::LEVEL_WARN)
#define LOG_ERROR   Log(Log::LEVEL_ERROR)

// flush after printing on windows to avoid console output buffering issues
#if defined( __WIN32__ ) || defined( _WIN32 )
#define LOG_FLUSH_COUT std::cout.flush();
#define LOG_FLUSH_CERR std::cerr.flush();
#else
#define LOG_FLUSH_COUT
#define LOG_FLUSH_CERR
#endif

/// filter using static Log::logLevel
/// note: storage and a default value needs to be set in a .cpp file
///       ex. Log::Level Log::logLevel = Log::LEVEL_NORMAL;
#ifdef LOG_STATIC_LEVEL
#define LOG_FILTER if(m_level < Log::logLevel) {return;}
#else
// otherwise always print, except debug is only available if DEBUG is defined
#define LOG_FILTER
#endif

/// \class Log
/// \brief a simple stream-based logger
///
/// class idea from:
/// http://www.gamedev.net/community/forums/topic.asp?topic_id=525405&whichpage=1&#3406418
/// how to catch std::endl (which is actually a func pointer):
/// http://yvan.seth.id.au/Entries/Technology/Code/std__endl.html
///
class Log {

	public:

		/// log level enum
		enum Level {
			LEVEL_DEBUG   = -2, ///< prints with DEBUG defined only
			LEVEL_VERBOSE = -1, ///< verbose info
			LEVEL_NORMAL  =  0, ///< normal info
			LEVEL_WARN    =  1, ///< warnings
			LEVEL_ERROR   =  2  ///< errors
		};

		#ifdef LOG_STATIC_LEVEL
		static Level logLevel; ///< levels below this will be filtered
		#endif

		/// select log level, default: normal
		Log(Level level=LEVEL_NORMAL) : m_level(level) {}

		/// does the actual printing on exit
		~Log() {
			LOG_FILTER
			switch(m_level) {
				case LEVEL_DEBUG:
					#if defined(LOG_STATIC_LEVEL) || defined(DEBUG)
					std::cout << "Debug: " << m_line.str();
					LOG_FLUSH_COUT
					#endif
					break;

				case LEVEL_VERBOSE:
					std::cout << m_line.str();
					LOG_FLUSH_COUT
					break;

				case LEVEL_NORMAL:
					std::cout << m_line.str();
					LOG_FLUSH_COUT
					break;

				case LEVEL_WARN:
					std::cerr << "Warn: " << m_line.str();
					LOG_FLUSH_CERR
					break;

				case LEVEL_ERROR:
					std::cerr << "Error: " << m_line.str();
					LOG_FLUSH_CERR
					break;
			}
		}

		/// catch << with a template class to read any type of data
		template <class T> Log& operator<<(const T &value) {
			m_line << value;
			return *this;
		}

		/// catch << ostream function pointers such as std::endl and std::hex
		Log& operator<<(std::ostream &(*func)(std::ostream&)) {
			func(m_line);
			return *this;
		}

	private:

		Log(Log const&);              // not defined, not copyable
		Log& operator = (Log const&); // not defined, not assignable

		Level m_level;                ///< log level
		std::ostringstream m_line;    ///< temp buffer
};
