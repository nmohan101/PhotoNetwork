simpleserver::simpleserver() {

	debug.enter("server", "simpleserver::simpleserver() {");

	init($());

	debug.exit("server", "} simpleserver::simpleserver()");
}
simpleserver::simpleserver(const jimhash& serveroptions) {

	//void (*theacceptcallback)(simpleclientsocket&), void (*thereceivemessagecallback)(simpleclientsocket&)) {
	debug.enter("server", "simpleserver::simpleserver() {");

	init(serveroptions);

	debug.exit("server", "} simpleserver::simpleserver()");
}
bool simpleserver::init(const jimhash& serveroptions) {

	debug.enter("server", "simpleserver::init() {");

	FD_ZERO(&activesockets);

	maxsocketid = 0;

	acceptcallback = 0;
	receivemessagecallback = 0;
	simplefullstack = false;
	applicationrequestcallback = 0;

	serveroptions.params(
		acceptcallback, "?acceptcallback",
		receivemessagecallback, "?receivemessagecallback",

		simplefullstack, "?simplefullstack",
		sharedstaticcontentfolder, "?sharedstaticcontentfolder",
		sharedstaticcontentfiles, "?sharedstaticcontentfiles",
		staticcontentfolder, "?staticcontentfolder",
		staticcontentfiles, "?staticcontentfiles",
		applicationrequestcallback, "?onapplicationrequest"
	);

	debug.exit("server", "} simpleserver::init()");

	return true;
}

bool simpleserver::startplaintext(int port) {

	debug.enter("server", "simpleserver::startplaintext(int) {");

	bool retrn = serversocket.plaintextlisten(port);

	debug.exit("server", "} simpleserver::sstartplaintext(int)");

	return retrn;
}

bool simpleserver::startssl(int port, const char* certfilename = "cert.crt", const char* keyfilename = "privatekey.key") {

	debug.enter("server", "simpleserver::startplaintext(int, const char*, const char*) {");

	if (!serversocket.loadcerts(certfilename, keyfilename)) {

		debug.fail("server", "} simpleserver::startplaintext(int, const char*, const char*)");

		return false;
	}

	bool retrn = serversocket.ssllisten(port);

	debug.exit("server", "} simpleserver::startplaintext(int, const char*, const char*)");

	return retrn;
}

void simpleserver::buildactivesocketfdlist() {

	debug.enter("server", "simpleserver::buildactivesocketfdlist() {");

	FD_ZERO(&activesockets);

	FD_SET(serversocket, &activesockets);
	maxsocketid = serversocket;

	for (int a = 0; a < clientsockets.size; a++) {

		simpleclientsocket& clientsocket = clientsockets[a];

		if (clientsocket.isconnected()) {

			FD_SET(clientsocket, &activesockets);

			if (clientsocket > maxsocketid) {

				maxsocketid = clientsocket;
			}
		}
		else {

			clientsockets.del(a--);
		}
	}

	debug.exit("server", "} simpleserver::buildactivesocketfdlist()");
}

bool simpleserver::serviceconnections() {

	debug.enter("server", "simpleserver::serviceconnections() {");

	buildactivesocketfdlist();

	if (select(maxsocketid + 1, &activesockets, 0, 0, 0) == -1) {

		debug.fail("server", "} simpleserver::serviceconnections()");

		return false;
	}

	// ToDo: optomize this so it isn't looping through all of the client sockets (maybe use epoll())

	if (FD_ISSET(serversocket, &activesockets)) {

		clientsockets += serversocket.acceptconnection();

		if (acceptcallback) {

			debug.print("server", "Calling callback function for Server Accept");

			acceptcallback(clientsockets[-1]);
		}
		else {

			debug.print("server", "No callback function for Server Accept");
		}
	}

	for (int a = 0; a < clientsockets.size; a++) {

		simpleclientsocket& clientsocket = clientsockets[a];

		if (FD_ISSET(clientsocket, &activesockets)) {

			if (simplefullstack) {

				debug.print("server", "Handling request as simplefullstack web request");

				onmessagereceiveddefault(clientsocket);
			}
			else if (receivemessagecallback) {

				debug.print("server", "Calling callback function for Server Message Receive");

				receivemessagecallback(clientsocket);
			}
			else {

				debug.print("server", "No method for handling requests to simplesever");
			}
		}
	}

	debug.exit("server", "} simpleserver::serviceconnections()");

	return true;
}

void simpleserver::onmessagereceiveddefault(simpleclientsocket& clientsocket) {


	debug.enter("server", "simpleserver::onmessagereceiveddefault(simpleclientsocket&)");

	jimhash request(simpleweb.parsehttprequest(clientsocket));

	//ToDo: check for failure and return an error request() if failed;

	//ToDo: handle all the shit about static content files
	//ToDo pass test versions of static content into server initialization. no need to abstact that out as a feature
	jimstring response;

	if (request.failed) {

		response = simpleweb.generateerrorhttpresponse();
	}
	else if (request.exists("disconnect")) {

		debug.print("server", "Disconnecting client");

		//ToDo: investigate why disconnect is a thing and if it should be
		clientsocket.disconnect();

		debug.exit("server", "simpleserver::onmessagereceiveddefault(simpleclientsocket&)");

		return;
	}
	else if (request.exists("error")) {

		//ToDo: make a simpledebug.istest and use that in all the simpleX classes
		if (istest) {

			msg("Error parsing request");
			msgln((jimstring)request["error"]);
		}

		response = simpleweb.generateerrorhttpresponse();
	}
	else {

		jimstring &resource = request["resource"];
		jimstring &data = request["data"];
		jimhash fdat;

		if (data.len) {

			fdat = simpleweb.parsequerystring(data.use());
		}

		if (resource == "/") {

			debug.print("server", "index.html request");

//---------ToDo: before using this, should make the billboard.js file create its container div and append it into document.body
//response = staticrequest(_("/billboard.html"));
			jimstring indexhtml;

			indexhtml = "<html><head>";

			for (int a = 0; a < sharedstaticcontentfiles.size; a++) {

				indexhtml += "<script src='" + sharedstaticcontentfiles[a] + "'></script>";
			}

			for (int a = 0; a < staticcontentfiles.size; a++) {

				indexhtml += "<script src='" + staticcontentfiles[a] + "'></script>";
			}

			indexhtml += "</head><body onload='init()' style='display: flex; flex-direction: column; overflow: hidden;'></body></html>";

			response = simpleweb.generatevalidhttpresponse(indexhtml);
////////////////////////////////////
		}
		else if (
			resource.seltext(-3) == ".js"
			|| resource.seltext(-4) == ".css"
		) {

			debug.print("server", "Static content request: ", resource);

			// User probably trying to get at file system. Send error
			// ToDo: Should probably log this user as a problme user for an eventual perge
			if (resource.find("..") > -1) {

				debug.print("server", "Static resource request contains a double period: \"", resource, "\"");

				response = simpleweb.generateerrorhttpresponse();
			}
			else {

				jimstring folder(
					sharedstaticcontentfiles.find(resource) > -1
					? sharedstaticcontentfolder
					: staticcontentfiles.find(resource) > -1
					? staticcontentfolder
					: ""
				);

				//ToDo: do caching with this function so you can just call it over and over
				jimstring currentpath(simplefilefunctions.getcurrentpath());

				if (folder.len) {

					jimstring fullpath(currentpath + folder + resource);

					//ToDo: caching of static content when not in test mode
					jimstring filedata(simplefilefunctions.readfile(fullpath));

					if (filedata.failed) {

						debug.print("server", "Could not open resource: \"", fullpath, "\"");

						response = simpleweb.generateerrorhttpresponse();
					}
					else {

						debug.print("server", "Retrieved ", _(filedata.len), " bytes of Static resource: \"", fullpath, "\"");

						//ToDo: this could be made faster with an rword parameter accepting from simpleweb.generatevalidhttpresponse
						response = simpleweb.generatevalidhttpresponse(filedata.give());
					}
				}
				else {

					debug.print("server", "Unknown static resource: \"", resource, "\"");

					response = simpleweb.generateerrorhttpresponse();
				}
			}
		}
		else if (resource == "/favicon.ico") {

			debug.print("server", "favicon.ico request");

			//ToDo handle favicon as well
			response = simpleweb.generateerrorhttpresponse();
		}
		else {

			debug.print("server", "Application request");

			if (applicationrequestcallback) {

				response = applicationrequestcallback(resource, fdat);
			}

			if (response.failed) {

				//ToDo: Have better defined error tiers and which ones have messages and which ones die and which ones just fail gracefully $
				if (istest) {

					msgln("RuntimeError: Unknown resource endpoint: \"" + resource + "\"");
				}

				response = simpleweb.generateerrorhttpresponse();
			}
		}
	}

	clientsocket.sendmessage(response);

	debug.exit("server", "simpleserver::onmessagereceiveddefault(simpleclientsocket&)");
}

bool simpleserver::adduser(const jimstring& username, const jimstring& password1, const jimstring& password2, jimstring& errorreturn) {

	jimstring error;

	if (users.exists(username)) {

		error += "\n    User already exists";
	}

	if (!username.regexmatch("^[a-zA-Z][_\\da-zA-Z]{0,19}$")) {

		error += "\n    Username must:\n        be 20 characters or less\n        start with a letter\n        be comprised of letters, numbers and/or underscores";
	}

	if (!password1.regexmatch("^.{1,20}$")) {

		error += "\n    Password must be 1-20 characters or less";
	}

	if (password1 != password2) {

		error += "\n    Passwords did not match";
	}

	if (error.len) {

		errorreturn = "Could not create new user:" + error;

		return false;
	}

	simpleuser newuser(username, password1);
	users[username] = newuser;

	return true;
}

bool simpleserver::login(const jimstring& username, const jimstring& password, const jimstring& accesskey, jimstring& accesskeyreturn, jimstring& errorreturn) {

	jimstring retrn;

	if (users.exists(username)) {

		simpleuser user = users[username];

		if (user.login(password, accesskey, accesskeyreturn, errorreturn)) {

			return true;
		}
	}
	else {

		errorreturn = "That username " + username + " does not exist. Check the spelling and try again, or "
			+ "click \"Create new account\" to create a new account.";
	}

	return false;
}
