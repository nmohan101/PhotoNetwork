
SimpleSSL::SimpleSSL() {

	islibraryinitialized = false;
}

bool SimpleSSL::initlibrary() {

	debug.enter("ssl", "SimpleSSL::initlibrary() {");

	if (!islibraryinitialized) {

		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		islibraryinitialized = true;

		debug.print("ssl", "Initialized library");
	}
	else {

		debug.print("ssl", "Library already previously initialized");
	}

	debug.exit("ssl", "} SimpleSSL::initlibrary()");

	return true;
}

bool SimpleSSL::initclientcontext() {

	debug.enter("ssl", "SimpleSSL::initclient() {");

	if (!clientcontext) {

		initlibrary();

		clientmethod = (SSL_METHOD*)TLSv1_2_client_method();
		clientcontext = SSL_CTX_new(clientmethod);

		if (!clientcontext) {

			debug.fail("ssl", "Could not create ssl client context");

			return false;
		}
		else {

			debug.print("ssl", "Created ssl context");
		}
	}
	else {

		debug.print("ssl", "Using existing ssl context");
	}

	debug.exit("ssl", "} SimpleSSL::initclient()");

	return true;

	// ToDo:
	//simple.uninitialize ssl
	// 	SSL_CTX_free(ctx);
}

bool SimpleSSL::initservercontext() {

	debug.enter("ssl", "SimpleSSL::initserver() {");

	if (!servercontext) {

		initlibrary();

		servermethod = (SSL_METHOD*)TLSv1_2_server_method();
		servercontext = SSL_CTX_new(servermethod);

		if (!servercontext) {

			debug.fail("ssl", "Could not create ssl server context");

			return false;
		}
		else {

			debug.print("ssl", "Created ssl context");
		}
	}
	else {

		debug.print("ssl", "Using existing ssl context");
	}

	debug.exit("ssl", "} SimpleSSL::initserver()");

	return true;

	// ToDo:
	//simple.uninitialize ssl
	// 	SSL_CTX_free(ctx);
}