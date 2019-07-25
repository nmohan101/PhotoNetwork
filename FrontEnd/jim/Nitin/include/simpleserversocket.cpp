

simpleserversocket::simpleserversocket() {

	debug.enter("serversocket", "simpleserversocket::simpleserversocket() {");

	// Initialize member variables to zero
	thesocket = 0;
	bzero(&thesockaddr, sizeof(thesockaddr));
	isssl = false;

	if (!simplessl.initservercontext()) {

		debug.fail("serversocket", "} simpleserversocket::simpleserversocket()");
	}

	debug.exit("serversocket", "} simpleserversocket::simpleserversocket()");
}

simpleserversocket::~simpleserversocket() {

}

simpleserversocket::operator int() {

	return thesocket;
}

bool simpleserversocket::loadcerts(const char* certfilename = "cert.crt", const char* keyfilename = "privatekey.key") {

	/* set the local certificate from CertFile */
	if ( SSL_CTX_use_certificate_file(simplessl.servercontext, certfilename, SSL_FILETYPE_PEM) <= 0 ) {

		ERR_print_errors_fp(stderr);
		return false;
	}

	/* set the private key from KeyFile (may be the same as CertFile) */
	if ( SSL_CTX_use_PrivateKey_file(simplessl.servercontext, keyfilename, SSL_FILETYPE_PEM) <= 0 ) {

		ERR_print_errors_fp(stderr);
		return false;
	}

	/* verify private key */
	if ( !SSL_CTX_check_private_key(simplessl.servercontext) ) {

		fprintf(stderr, "Private key does not match the public certificate\n");
		return false;
	}

    return true;
}

bool simpleserversocket::plaintextlisten(int serverport) {

	if (0) {

		return false;
	}

	return true;
}

bool simpleserversocket::ssllisten(int serverport) {

	debug.enter("serversocket", "simpleserversocket::ssllisten(int) {");

	if (serverport < 1) {

		debug.fail("serversocket", "} simpleserversocket::ssllisten(int) - Invalid listening port: ", _(serverport).use());

		return false;
	}

	// Create the listening socket
	thesocket = socket(PF_INET, SOCK_STREAM, 0);

	// Prevent the socket from staying locked for a while after the socket is closed
	int dumboptval = 1;
	setsockopt(thesocket, SOL_SOCKET, SO_REUSEADDR, (void*)&dumboptval, sizeof(dumboptval));

	//ToDo: Make this an option
	// Set to non-blocking to avoid hanging
	//fcntl(thesocket, F_SETFL, fcntl(thesocket, F_GETFL, 0) | O_NONBLOCK);

	// Create the socket port configuration
	thesockaddr.sin_family = AF_INET;
	thesockaddr.sin_port = htons(serverport);
	thesockaddr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the port configuration
	if (bind(thesocket, (struct sockaddr*)&thesockaddr, sizeof(thesockaddr))) {

		thesocket = 0;

		debug.fail("serversocket", "} simpleserversocket::ssllisten(int) - Could not bind to port: ", _(serverport).use());

		return false;
	}

	// Listen for connections on the socket
	if (listen(thesocket, 50)) {

		thesocket = 0;

		debug.fail("serversocket", "} simpleserversocket::ssllisten(int) - Could not configure listening socket on port: ", _(serverport).use());

		return false;
	}

	isssl = true;

	debug.exit("serversocket", "} simpleserversocket::ssllisten(int)");

	return true;
}

bool simpleserversocket::websocketlisten(int serverport) {

	if (0) {

		return false;
	}

	return true;
}

void simpleserversocket::disconnect() {

	if (thesocket) {

		close(thesocket);
	}
}

bool simpleserversocket::connectionwaiting() {


}

simpleclientsocket simpleserversocket::acceptconnection() {

	debug.enter("serversocket", "simpleserversocket::acceptconnection() {");

	simpleclientsocket retrn;

	socklen_t dumbsocklen = sizeof(retrn.thesockaddr);

	// Create client socket representing the new connection
	retrn.thesocket = accept(thesocket, (sockaddr*)&retrn.thesockaddr, &dumbsocklen);

	if (retrn.thesocket == -1) {

		// accept() failed
		retrn.thesocket = 0;

		debug.fail("serversocket", "} simpleserversocket::acceptconnection() - Could not create Client Socket Connection");

		return retrn;
	}

	if (isssl) {

		// Create SSL connection
		retrn.sslconnection = SSL_new(simplessl.servercontext);

		if (retrn.sslconnection) {

			// Associate the SSL connection with the new client socket
			int sslsetfdresult = SSL_set_fd(retrn.sslconnection, retrn.thesocket);

			if (sslsetfdresult) {

				int sslacceptresult = SSL_accept(retrn.sslconnection);

				if (sslacceptresult > 0) {

					debug.print("serversocket", "New Client SSL Connection: ", inet_ntoa(retrn.thesockaddr.sin_addr), ":", _(ntohs(retrn.thesockaddr.sin_port)).use());

					debug.exit("serversocket", "} simpleserversocket::acceptconnection()");

					return retrn;
				}
				else {

					int errorcode = SSL_get_error(retrn.sslconnection, sslacceptresult);
					jimstring errormessage;
					if (errorcode == SSL_ERROR_NONE) errormessage = "SSL_ERROR_NONE: No error to report. This is set when the value of the ret parameter is greater than 0.";
					else if (errorcode == SSL_ERROR_SSL) errormessage = "SSL_ERROR_SSL: An error occurred in the SSL library.";
					else if (errorcode == SSL_ERROR_WANT_READ) errormessage = "SSL_ERROR_WANT_READ: Processing was not completed successfully because there was no data available for reading, and the socket available for the SSL session is in nonblocking mode. Try the function again at a later time.";
					else if (errorcode == SSL_ERROR_WANT_WRITE) errormessage = "SSL_ERROR_WANT_WRITE: Processing was not completed successfully because the socket associated with the SSL session is blocked from sending data. Try the function again at a later time.";
					else if (errorcode == SSL_ERROR_SYSCALL) errormessage = "SSL_ERROR_SYSCALL: An I/O error occurred. Issue the sock_errno function to determine the cause of the error.";
					else if (errorcode == SSL_ERROR_ZERO_RETURN) errormessage = "SSL_ERROR_ZERO_RETURN: The remote application shut down the SSL connection normally. Issue the SSL_shutdown function to shut down data flow for an SSL session.";
					else if (errorcode == SSL_ERROR_WANT_CONNECT) errormessage = "SSL_ERROR_WANT_CONNECT: Processing was not completed successfully because the SSL session was in the process of starting the session, but it has not completed yet. Try the function again at a later time.";

					debug.print("serversocket", "SSL_accept() Error(", _(errorcode).use(), ") - ", errormessage.use());
				}
			}
			else {

				debug.print("serversocket", "SSL_set_fd() Error");
			}
		}
		else {

			debug.print("serversocket", "SSL_new() Error");
		}
	}
	else {

		debug.print("serversocket", "New Client SPlaintext Socket Connection: ", inet_ntoa(retrn.thesockaddr.sin_addr), ":", _(ntohs(retrn.thesockaddr.sin_port)).use());

		debug.exit("serversocket", "} simpleserversocket::acceptconnection()");

		return retrn;
	}

	jimstring errorresult(ERR_error_string(ERR_get_error(), 0));

	debug.print("serversocket", "Error result: ", errorresult.use());

	retrn.disconnect();

	//ToDo: Need to unaccept the normal socket connection?
	debug.fail("serversocket", "} simpleserversocket::acceptconnection() - Could not create SSL Connection");

	return retrn;
}

bool operator==(const simpleclientsocket& sock1, const simpleserversocket& sock2) {

	if (0) {

		return false;
	}

	return true;
}