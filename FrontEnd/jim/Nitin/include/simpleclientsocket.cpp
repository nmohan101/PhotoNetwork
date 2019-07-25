
simpleclientsocket::simpleclientsocket() {

	debug.enter("clientsocket", "simpleclientsocket::simpleclientsocket() {");

	init();

	debug.exit("clientsocket", "} simpleclientsocket::simpleclientsocket()");
}
simpleclientsocket::simpleclientsocket(int newsocket) {

	debug.enter("clientsocket", "simpleclientsocket::simpleclientsocket(int) {");

	init();

	thesocket = newsocket;

	debug.exit("clientsocket", "} simpleclientsocket::simpleclientsocket(int)");
}
void simpleclientsocket::init() {

	debug.enter("clientsocket", "simpleclientsocket::init() {");

	thesocket = 0;
	sslconnection = 0;

	if (!simplessl.initclientcontext()) {

		debug.fail("clientsocket", "} simpleclientsocket::init()");
	}

	debug.exit("clientsocket", "} simpleclientsocket::init()");
}

simpleclientsocket::operator int() {

	return thesocket;
}


bool simpleclientsocket::plaintextconnect(const char* ipaddress, int serverport) {

	debug.enter("clientsocket", "simpleclientsocket::plaintextconnect(const char*, int) {");

	if (_(ipaddress) == "") {

		debug.fail("clientsocket", "} simpleclientsocket::plaintextconnect(const char*, int) - Missing ipaddress");
		return false;
	}

	thesocket = socket(PF_INET, SOCK_STREAM, 0);

	thesockaddr.sin_family = AF_INET;
	thesockaddr.sin_port = htons(serverport);
	thesockaddr.sin_addr.s_addr = inet_addr(ipaddress);

	if (connect(thesocket, (sockaddr*)&thesockaddr, sizeof(thesockaddr))) {

		debug.fail("clientsocket", "} simpleclientsocket::plaintextconnect(const char*, int) - Connection failed");

		disconnect();

		return false;
	}
// Win 32 Socket event
//		WSAAsyncSelect (thesocket, thehandle, simpleevent.WM_ONSOCKET, FD_READ | FD_WRITE | FD_CLOSE);

	debug.print("clientsocket", "Socket connected to ", ipaddress);

	debug.exit("clientsocket", "} simpleclientsocket::plaintextconnect(const char*, int)");

	return true;
}
bool simpleclientsocket::sslconnect(const char* ipaddress, int serverport) {

	debug.enter("clientsocket", "simpleclientsocket::sslconnect(int, const char*, int) {");

	if (!plaintextconnect(ipaddress, serverport)) {

		debug.fail("clientsocket", "} simpleclientsocket::sslconnect(int, const char*, int) - Socket Connection Failed");

		disconnect();

		return false;
	}

	sslconnection = SSL_new(simplessl.clientcontext);

	if (!sslconnection) {

		debug.fail("clientsocket", "} simpleclientsocket::sslconnect(int, const char*, int) - SSL Connection Failed to Initialize");

		disconnect();

		return false;
	}

	SSL_set_fd(sslconnection, thesocket);

	if (SSL_connect(sslconnection) == -1) {

		debug.fail("clientsocket", "} simpleclientsocket::sslconnect(int, const char*, int) - SSL Handshake failed");

		disconnect();

		return false;
	}

	debug.print("clientsocket", "Established ssl handshake with ", ipaddress);

	debug.exit("clientsocket", "} simpleclientsocket::sslconnect(int, const char*, int)");

	return true;
}
bool simpleclientsocket::websocketconnect(const char* ipaddress, int serverport) {

	debug.enter("clientsocket", "simpleclientsocket.websocketconnect");

	debug.exit("clientsocket", "}");

	return true;
}

bool simpleclientsocket::isconnected() {

	return thesocket != 0;
}

void simpleclientsocket::disconnect() {

	if (sslconnection) {

		SSL_free(sslconnection);
		sslconnection = 0;
	}

	if (thesocket) {

		shutdown(thesocket, 0x02);
		close(thesocket);
		thesocket = 0;
	}
}

jimstring simpleclientsocket::getserversslcertificate() {

	debug.exit("simpleclientsocket::getserversslcertificate()");

	X509 *certificate;
	char *subjectname, *issuername;
	jimstring retrn;

	if (!sslconnection) {

		debug.fail("No SSL conntection");

		retrn.fail();

		return retrn;
	}

	certificate = SSL_get_peer_certificate(sslconnection);

	if (!certificate) {

		debug.fail("could not get certificate");

		retrn.fail();
		return retrn;
	}
	else {

		subjectname = X509_NAME_oneline(X509_get_subject_name(certificate), 0, 0);
		issuername = X509_NAME_oneline(X509_get_issuer_name(certificate), 0, 0);

		retrn = "Subject Name: " + _(subjectname) + "\n" + "Issuer Name: " + _(issuername);

		free(subjectname);
		free(issuername);

		X509_free(certificate);
	}

	debug.exit("simpleclientsocket::getserversslcertificate()");

	return retrn;
}

bool simpleclientsocket::sendmessage(const char* text, int length) {

	debug.enter("clientsocket", "simpleclientsocket::sendmessage(const char*, int)");

	bool success = true;
	int bytessent = 0;

	if (!thesocket) {

		debug.fail("clientsocket", "} Not Connected");

    	return false;
	}

	if (sslconnection) {

		// SSL Send

		debug.print("clientsocket", ("Attempting to send " + _(length) + " bytes via SSL").use());
		bytessent = SSL_write(sslconnection, text, length);

		if (bytessent) {

			debug.print("clientsocket", ("Successfully sent " + _(bytessent) + " bytes via SSL").use());
		}

		success = bytessent > 0;

	}
	else {

		// Plaintext Send

		debug.print("clientsocket", ("Attempting to send " + _(length) + " bytes via Plain Text").use());
		bytessent = send(thesocket, text, length, 0);

		if (bytessent > -1) {

			debug.print("clientsocket", ("Successfully sent " + _(bytessent) + " bytes via Plain Text").use());
		}

		success = bytessent > -1;
	}

	if (success) {

		debug.exit("clientsocket", "} Message Sent");
	}
	else {

		debug.fail("clientsocket", "}");
	}

	return success;
}
bool simpleclientsocket::sendmessage(const jimstring& text) {

    return sendmessage(text.use(), text.len);
}

jimstring simpleclientsocket::getmessage(int maxsize) {

	debug.enter("clientsocket", "simpleclientsocket::getmessage(int)");

	bool success = true;
	int bytesreceived;
	jimstring retrn(maxsize);

	if (thesocket) {

		if (sslconnection) {

			// SSL Get

			int bytesreceived = SSL_read(sslconnection, retrn.use(), retrn.len);

			retrn.truncate(bytesreceived);

			debug.print("clientsocket", "Successfully read ", _(bytesreceived).use(), " bytes via SSL");
		}
		else {

			// Plaintext Get
			debug.print("clientsocket", "PLAINTEXT NOT YET CODED");

			success = false;
		}
	}

	if (success) {

		debug.exit("clientsocket", "} Message Received");
	}
	else {

		debug.fail("clientsocket", "}");
	}

	return retrn;
}

jimstring simpleclientsocket::getmessagesmart() {

	jimstring retrn;
/*
	for (int a = 0; a < simple.sockets.size; a++) {

		if (thesocket == simple.sockets[a]) {

			retrn = simple.socketmessages[a]->give();
		}
	}
*/
	return retrn;
 }

jimstring simpleclientsocket::getip()
{
	jimstring temp, retrn;
/*
	temp = inet_ntoa(otherinfo.sin_addr);
	retrn = simple.stringtoip(temp.array);
*/
	return retrn;
}

jimstring simpleclientsocket::getipstring()
{
	jimstring retrn;


//retrn = inet_ntoa(otherinfo.sin_addr);

	return retrn;
}

////////////////////////////////
//////////////////////////////// windowsfunctions
////////////////////////////////
/*
simpleclientsocket simpleadult::createclientsocket()
{
    simpleclientsocket retrn;
    id theid;

	retrn.thehandle = thehandle;
	return retrn;
}
*/