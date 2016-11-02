/*==============================================================================

	PathWatcher.h

	Copyright (C) 2016 Dan Wilcox <danomatika@gmail.com>
	
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
	
	Adapted from https://github.com/NickHardeman/ofxFileWatcher

==============================================================================*/
#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <sys/stat.h>

/// \class PathWatcher
/// \brief watch file and directory paths for modifications
///
/// detects creation, modification, and deletion/move events
///
/// Example usage:
///
///     PathWatcher watcher;
///
///     // add a path to watch
///	    watcher.addPath("test.txt");
///
///     // start the thread, otherwise call update() to check manually
///    	watcher.start();
///
///     // set callback as a function pointer or lambda
///     watcher.setCallback([](const PathWatcher::Event &event) {
///         switch(path.change) {
///             case PathWatcher::CREATED:
///                 std::cout << "path created: " << event.path << std::endl;
///                 break;
///             case PathWatcher::MODIFIED:
///                 std::cout << "path modified: " << event.path << std::endl;
///                 break;
///             case PathWatcher::DELETED:
///                 std::cout << "path deleted: " << event.path << std::endl;
///                 break;
///             default: // NONE
///                 break;
///         }
///     });
///
class PathWatcher {

	public:

		PathWatcher() {}
		virtual ~PathWatcher() {stop();}

	/// \section Paths

		/// add a path to watch, full or relative to current directory
		/// optionally set contextual name
		void addPath(const std::string &path, const std::string &name="") {
			if(!pathExists(path)) {return;}
			mutex.lock();
			std::vector<Path>::iterator iter = std::find_if(paths.begin(), paths.end(),
				[&path](Path const &p) {
					return p.path == path;
				}
			);
			if(iter == paths.end()) {
				paths.push_back(Path(path, name));
			}
			mutex.unlock();
		}

		/// remove a watched path
		void removePath(const std::string &path) {
			mutex.lock();
			std::vector<Path>::iterator iter = std::find_if(paths.begin(), paths.end(),
				[&path](Path const &p) {
					return p.path == path;
				}
			);
			if(iter != paths.end()) {
				paths.erase(iter);
			}
			mutex.unlock();
		}
	
		/// remove a watched path by name
		void removePathByName(const std::string &name) {
			mutex.lock();
			std::vector<Path>::iterator iter = std::find_if(paths.begin(), paths.end(),
				[&name](Path const &p) {
					return p.name == name;
				}
			);
			if(iter != paths.end()) {
				paths.erase(iter);
			}
			mutex.unlock();
		}

		/// remove all watched paths
		void removeAllPaths() {
			mutex.lock();
			paths.clear();
			mutex.unlock();
		}
	
		/// does a path exist?
		static bool pathExists(const std::string & path) {
			return access(path.c_str(), F_OK) == 0;
		}

	/// \section Watching
	
		/// the type of change
		enum ChangeType {
			NONE,     //< path has not changed
			CREATED,  //< path was created
			MODIFIED, //< path was modified
			DELETED   //< path was deleted or moved
		};
	
		/// a change event
		struct Event {
			ChangeType change; //< type of change: created, modified, deleted
			std::string path;  //< path
			std::string name;  //< optional contextual name
		};

		/// set callback to receive change events
		///
		/// function:
		///
		///     void pathChanged(const PathWatcher::Event &event) {
		///         cout << "path changed: " << event.path << endl;
		///     }
		///
		///     int main() {
		///         PatchWatcher watcher;
		///         watcher.setCallback(pathChanged);
		///         watcher.start();
		///         ...
		///     }
		///
		/// class member function:
		///
		///     class Foo {
		///         public:
		///             void setup();
		///             void pathChanged(const PathWatcher::Event &event);
		///     };
		///     ...
		///     void Foo::setup() {
		///         std::function<void(const PathWatcher::Event &)> cb;
		///
		///         // via lambda
		///         cb = [this](const PathWatcher::Event &event) {
		///             pathChanged(event);
		///         };
		///
		///         // or function pointer
		///         cb = std::bind(&Foo::pathChanged, this, std::placeholders::_1);
		///
		///         PatchWatcher watcher;
		///         watcher.setCallback(cb);
		///         watcher.start();
		///         ...
		///     }
		///
		void setCallback(std::function<void(const PathWatcher::Event &event)> const &callback) {
			mutex.lock();
			this->callback = callback;
			mutex.unlock();
		}
	
		/// remove deleted paths automatically? (default: false)
		void setRemoveDeletedPaths(bool remove) {
			removeDeleted = remove;
		}

		/// manually check for changes and call callback function for each modified path,
		/// if a file has been deleted it is removed
		void update() {
			mutex.lock();
			auto iter = std::begin(paths);
			while(iter != std::end(paths)) {
				Path &path = (*iter);
				ChangeType change = path.changed();
				if(change != NONE) {
					if(callback) {
						Event event;
						event.change = change;
						event.path = path.path;
						event.name = path.name;
						callback(event);
					}
					if(change == DELETED && removeDeleted) {
						paths.erase(iter);
						continue;
					}
				}
				iter++;
			}
			mutex.unlock();
		}

		/// start a background thread to automatically check for changes,
		/// sleep sets how often to check in ms
		void start(unsigned int sleep=500) {
			if(!running) {
				running = true;
				thread = new std::thread([this,sleep]{
					while(running) {
						update();
						std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
					}
				});
			}
		}

		/// stop the background thread
		void stop() {
			if(running) {
				running = false;
				thread->join();
				delete thread;
				thread = nullptr;
			}
		}
	
		/// is the thread currently running?
		bool isRunning() {return running;}

	protected:
	
		/// a path to watch
		class Path {
			
			public:
			
				std::string path;    //< relative or absolute path
				std::string name;	 //< optional contextual name
				long modified = 0;   //< last modification st_mtime
				bool exists = true;  //< does the path exist?
			
				/// create a new Path to watch with optional name
				Path(const std::string &path, const std::string &name="") {
					this->path = path;
					this->name = name;
					if(pathExists(path)) {
						update();
					}
					else {
						exists = false;
					}
				}
			
				/// returns detected change type or NONE
				ChangeType changed() {
					if(pathExists(path)) {
						if(exists) {
							struct stat attributes;
							stat(path.c_str(), &attributes);
							if(modified != attributes.st_mtime) {
								modified = attributes.st_mtime;
								return MODIFIED;
							}
						}
						else {
							update();
							exists = true;
							return CREATED;
						}
					}
					else if(exists) {
						modified = 0;
						exists = false;
						return DELETED;
					}
					return NONE;
				}
			
				/// update modification time
				void update() {
					struct stat attributes;
					stat(path.c_str(), &attributes);
					modified = attributes.st_mtime;
				}
		};

		bool running = false;          //< is the thread running?
		std::thread *thread = nullptr; //< thread
		std::mutex mutex;              //< thread data mutex
		std::vector<Path> paths;       //< paths to watch
		bool removeDeleted = false;    //< remove path when deleted?
	
		/// change event callback function pointer
		std::function<void(const PathWatcher::Event &event)> callback = nullptr;
};
