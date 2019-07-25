
#ifdef _WIN32

	#define NOMINMAX
	#include <windows.h>

#elif __linux__

	typedef int HWND;
	typedef int SOCKET;

#endif

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#ifdef _WIN32

	#include <time.h>
	#include <math.h>

#endif

// Output
	#include "simplemsg.h"

// Filestream
#include <fstream>

// Open SSL
	#include <openssl/ssl.h>
	#include <openssl/err.h>
	#include <openssl/md5.h>

// 64 Bit ints
	#include <inttypes.h>
	typedef int64_t __int64;

// String Functions
	#include <string.h>
	#include <regex>


//delete these 2:
	//#include "simpleinfile.h"
	//#include "simpleoutfile.h"

#include "jimstring.h"
#include "simpledebug.h"

// Data Types
#include <utility> // std::move

#include "jimvector.h"

#include "simpledebug.cpp"
simpledebug debug;

#include "jimitem.h"
#include "jimvector.cpp"

#include "jimhash.h"
#include "jimstring.cpp"
	#include "simplestring.h"
	#include "simplestring.cpp"

#include <typeinfo>
#include "jimitem.cpp"

#include "jimhash.cpp"
typedef jimhash $;

#include "simplejson.h"
#include "simplejson.cpp"

#include "debugobject.h"
#include "debugobject.cpp"

//#include "simpleidheader.h"
//#include "simpleid.h"

//System

	//Needed for sysconf()
	#include <unistd.h>

	#ifdef WINDOWS
	    #include <direct.h>
	#elif __linux__
	    #include <unistd.h>
	#endif

	#include "simplesystem.h"
	#include "simplesystem.cpp"

// File System
	#include "simplefile.h"
	#include "simplefile.cpp"
	#include <limits.h>
	#include <libgen.h>
	#include "simplefilefunctions.h"
	#include "simplefilefunctions.cpp"

//Web
	#include <errno.h>
	#include <malloc.h>
	#include <sys/socket.h>
	#include <resolv.h>
	#include <netdb.h>
	#include <fcntl.h>

	// Needed for inet_addr()
		#include <arpa/inet.h>

		#include "simplessl.h"
		#include "simplessl.cpp"

	// Simple Socket Objects
		#include "simpleclientsocket.h"
		#include "simpleclientsocket.cpp"
		#include "simpleserversocket.h"
		#include "simpleserversocket.cpp"
	// Simple Functions
		#include "simpleweb.h"
		#include "simpleweb.cpp"
	// Simple Server Object
		#include "simpleuser.h"
		#include "simpleuser.cpp"
		#include "simpleserver.h"
		#include "simpleserver.cpp"

	//uWebRequest
		/*#include "uWS/uWS.h"
		#include "uWS/Hub.cpp"
		#include "uWS/Epoll.cpp"
		#include "uWS/Extensions.cpp"
		#include "uWS/Group.cpp"
		#include "uWS/HTTPSocket.cpp"
		#include "uWS/Networking.cpp"
		#include "uWS/Node.cpp"
		#include "uWS/Socket.cpp"
		#include "uWS/WebSocket.cpp"*/



/*
#ifdef _WIN32


	#include <shlobj.h>

#endif
*/
