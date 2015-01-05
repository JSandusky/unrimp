SimpleFileWatcher - SVN revision 17
- License: "MIT"
- Online: https://code.google.com/p/simplefilewatcher/
- Reasons for usage of this library
	- "Poco::DirectoryWatcher" is overkill when it comes to watching multiple directories:
		- No recursive watch support
		- One thread per directory watcher (thread spamming which makes it hard to debug a large scale application with something like this in place)
	- Qt "QFileSystemWatcher": We would now depend on the huge Qt core library, just for this
