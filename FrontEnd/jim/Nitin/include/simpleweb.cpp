jimstring SimpleWeb::getipfromurl(const char* url) {

	debug.enter("web", "SimpleWeb::getipfromurl()");

	jimstring retrn;

	hostent *host = gethostbyname(url);

	if (host) {

		retrn = inet_ntoa(*((struct in_addr *) host->h_addr_list[0]));

		debug.exit("web", "getipfromurl() resolved: ", retrn.use());
	}
	else {

		retrn.fail();

		debug.fail("web", "SimpleWeb::getipfromurl() Could not resolve");
	}

	return retrn;
}

jimhash SimpleWeb::request(simpleclientsocket& clientsocket, const char* host, const char* operation, const char* postdata = 0, const jimhash& header = $()) {

	debug.enter("web", "SimpleWeb::request(simpleclientsocket&, const char*, const char*, const jimhash&) {");

	jimhash retrn;
	jimstring response;
	jimhash responseheader;

	jimstring themsg;

	themsg = _(operation) + " HTTP/1.1\r\n";
	themsg += "Host: " + _(host) + "\r\n";

	if (header.size) {

		// Build header string

		jimvector<jimstring> keys = header.getkeys();

		debug.print("web", "Building header string with ", _(keys.size).use(), " keys");

		for (int a = 0; a < keys.size; a++) {

			jimstring &key = keys[a];

			debug.print("web", "Adding header item: ", key.use());

			jimstring entry = (jimstring)header[key];

			debug.print("web", "Header Entry Value: ", entry.use());

			themsg += key + ": " + entry + "\r\n";
		}
	}
	else {

		debug.print("web", "No Header elements passed");
	}

	themsg += "\r\n";

	if (postdata) {

		themsg += postdata;
	}

	clientsocket.sendmessage(themsg);

	jimstring chunk;
	jimstring total;

	int datapos = 0;
	int headersize = 0;
	int chunkheaderstartpos = 0;

	bool fullchunkfound = false;
//ToDo: Not necesarily chunked, could be content-length
//ToDo: limit to 1gb before quitting and returning fail
//ToDo: Build response header
//		Need jimstring split and join

	// Loop until the secondary loop returns after finding a full chunk.
	// This will be the last chunk because the second loop usually ends without having
	// just completed a full chunk unless it has encountered the last chunk header.
	do {

		chunk = clientsocket.getmessage(10000);

		if (!chunk.len) {

			debug.fail("web", "} Received incomplete response");

			return retrn.fail();
		}

		debug.print("web", "Analyzing message...");

		total += chunk.give();

		if (!headersize) {

			if (total.findandmove("\r\n\r\n", datapos) > -1) {

				headersize = datapos - 4;
				chunkheaderstartpos = datapos;

				jimstring headerreceived = total.seltext(0, headersize);
//ToDo: parse header and populate a return hash if one is passed
				debug.print("web", "Received ", _(headersize).use(), " bytes of header");
			}
			else {

				datapos = 0;

				debug.print("web", "End of header not received yet.");
			}
		}

		if (headersize) {

			datapos = chunkheaderstartpos;

			// We are passed the header. Loop to collect any complete chunks that were received

			do {

				fullchunkfound = false;

				int chunkheaderendpos = total.findandmove("\r\n", datapos);

				if (chunkheaderendpos > -1) {

					// Full chunk header

					jimstring chunkstring = total.seltext(chunkheaderstartpos, chunkheaderendpos - chunkheaderstartpos);

					// Strip options from chunk header

					int extpos = chunkstring.find(";", 0);

					if (extpos > -1) {

						chunkstring = chunkstring.seltext(0, extpos);
					}

					debug.print("web", "Found chunk header. hex size: ", chunkstring.use());

					int chunksize = 0;

//ToDo: Encapsulate hex string conversion
					for (int a = 0; a < chunkstring.len; a++) {

						int weight = 16 * (chunkstring.len - 1 - a);

						char thechar = chunkstring[a];

						int value = (
							(thechar >= 97 && thechar <= 102)
							? thechar - 87
							: thechar - 48
						);

						chunksize += value * (weight ? weight : 1);
					}

					debug.print("web", "Chunk size in decimal: ", _(chunksize).use());

					if (chunksize > 0) {

						if (datapos + chunksize <= total.len) {

							// Full chunk has been received

							jimstring datachunk = total.seltext(datapos, chunksize);

							debug.print("web", "Receved chunk of size ", _(datachunk.len).use());

							response += datachunk.give();

							chunkheaderstartpos = datapos + chunksize;
							fullchunkfound = true;
						}
						else {

							debug.print("web", "Full chunk not received yet. Getting more data...");
						}
					}
					else {

						// End of transmission found

						fullchunkfound = true;
						break;
					}
				}
			} while (fullchunkfound);
		}
	} while (!fullchunkfound);

	debug.print("web", "Received ", _(total.len).use(), " bytes of data total:\n", total.use());
	debug.print("web", "Successful response of ", _(response.len).use(), " bytes:\n", response.use());
	debug.exit("web", "} SimpleWeb::request(simpleclientsocket&, const char*, const char*, const jimhash&)");

	retrn["response"] = response.give();
	retrn["header"] = responseheader.give();

	return retrn;
}

jimhash SimpleWeb::httpsrequest(const char* host, const char* operation, const char* postdata = 0, const jimhash& header = $()) {

	debug.enter("web", "SimpleWeb::httpsrequest(const char*, int, const char*, const char*, const jimhash&) {");

	jimhash retrn;

	simpleclientsocket clientsock;

	jimstring ip = getipfromurl(host);
//ToDo:: What if host is bad url
	if (clientsock.sslconnect(ip.use(), 443)) {

		debug.print("web", "Connected to ", ip.use());

		jimhash retrn = simpleweb.request(
			clientsock,
			host,
			operation,
			postdata,
			header
		);

		if (!retrn.failed) {

			debug.exit("web", "} SimpleWeb::httpsrequest(const char*, int, const char*, const char*, const jimhash&)");

			return retrn;
		}
	}

	debug.fail("web", "} SimpleWeb::httpsrequest(const char*, int, const char*, const char*, const jimhash&)");

	retrn.fail();
	return retrn;
}

jimhash SimpleWeb::parsehttprequest(simpleclientsocket& clientsocket) {

	debug.enter("web", "SimpleWeb::parsehttprequest(const char*) {");

	jimhash retrn;

	jimstring request = clientsocket.getmessage(3000);
// ToDo: Make sure that the system isn't hanging and waiting for a message that isn't coming (among other attack vectors)
	if (!request.len) {

		retrn["disconnect"] = _("1");

		debug.exit("web", "} SimpleWeb::parsehttprequest(const char*)");

		return retrn;
	}

	int datastartpos = 0;
	int headerendpos = request.findandmove("\r\n\r\n", datastartpos);

	if (headerendpos < 0) {

		retrn["error"] = _("invalid request");

		debug.fail("web", "} SimpleWeb::parsehttprequest(const char*)");

		return retrn;
	}

	//jimstring data = request.seltext(datastartpos);
	//retrn["data"] = data;

	debug.print("web", "Got: ", _(headerendpos).use(), " bytes of header data");

	//ToDo: Use position marker instead of spliting for performance
	jimvector<jimstring> headerlines = request.seltext(0, headerendpos).split("\r\n");

	jimvector<jimstring> items = headerlines[0].split(" ");

	retrn["operation"] = items[0];
	retrn["resource"] = items[1];
	retrn["protocol"] = items[2];

	jimhash header;

	debug.print("web", "About to retrieve: ", _(headerlines.size - 1).use(), " header rows");

	for (int a = 1; a < headerlines.size; a++) {

		jimstring &headerline = headerlines[a];

		debug.print("web", "parsing header line: ", headerline.use());

		int headervaluestart = 0;
		int headerkeyend = headerline.findandmove(": ", headervaluestart);

		if (headerkeyend == -1) {

			retrn["error"] = _("invalid header item");

			debug.fail("web", "} SimpleWeb::parsehttprequest(const char*)");

			return retrn;
		}

		jimstring key = headerline.seltext(0, headerkeyend);

		jimstring value = headerline.seltext(headervaluestart);

		debug.print("web", "Got key/value: ", key.use(), "/", value.use());

		header[key] = value;
	}

	retrn["header"] = header;

	if (header.exists("Content-Length")) {

		jimstring contentlengthstring = (jimstring)header["Content-Length"];

		if (!contentlengthstring.isint()) {

			retrn["error"] = _("Non-integer Content-Length");

			debug.exit("web", "} SimpleWeb::parsehttprequest(const char*)");

			return retrn;
		}

		debug.print("web", "Content-Length: ", contentlengthstring.use());

		int contentlength = contentlengthstring.toint();

		while (contentlength > request.len - datastartpos) {

			debug.print("web", "Received ", _(request.len - datastartpos).use(), "/", contentlengthstring.use(), " bytes of data");

			request += clientsocket.getmessage(3000);
		}

		debug.print("web", "Received ", _(request.len - datastartpos).use(), "/", contentlengthstring.use(), " bytes of data");

		retrn["data"] = request.seltext(datastartpos);
	}
	else {

		retrn["data"] = _("");
	}

	debug.exit("web", "} SimpleWeb::parsehttprequest(const char*)");

	return retrn;
}

jimhash SimpleWeb::parsequerystring(const char* querystring) {

	debug.enter("web", "} SimpleWeb::parsefdat(const char*)");
//ToDo: Harden this function
	jimhash retrn;

	//ToDo: Something wrong with borrowing this. Figure out why
	//jimstring myfdat;
	//myfdat.borrow(fdat);
	jimstring myfdat(querystring);

	debug.print("web", "Parsing: ", myfdat.use());

	jimvector<jimstring> entries = myfdat.split("&");

	for (int a = 0; a < entries.size; a++) {

		jimstring &entry = entries[a];

		debug.print("web", "Parsing entry: ", entry.use());

		jimvector<jimstring> keyvalue = entry.split("=");

		debug.print("web", "Parsed: ", keyvalue[0].use(), "/", keyvalue[1].use());

		retrn[simpleweb.urldecode(keyvalue[0].use())] = simpleweb.urldecode(keyvalue[1].use());
	}

	debug.exit("web", "} SimpleWeb::parsefdat(const char*)");

	return retrn;
}

jimstring SimpleWeb::converttoquerystring(const jimhash& fdat) {

	jimstring retrn;

	jimvector<jimstring> thekeys = fdat.getkeys();

	for (int a = 0; a < thekeys.size; a++) {

		jimstring &key = thekeys[a];
		jimstring &value = fdat[key];

		if (a) {

			retrn += "&";
		}

		retrn += simpleweb.urlencode(key) + "=" + simpleweb.urlencode(value);
	}

	return retrn;
}

jimstring SimpleWeb::generatevalidhttpresponse(const char* data, const jimhash& header) {

	jimstring retrn;

	int contentlength = data ? (int)strlen(data) : 0;

	retrn = "HTTP/1.1 200 OK\r\n";
	retrn += "Content-Type: text/html\r\n";
	retrn += "Content-Length: " + _(contentlength) + "\r\n";
	retrn += "Connection: close\r\n";
	retrn += "\r\n";

	if (data) {

		retrn += data;
	}

	return retrn;
}

jimstring SimpleWeb::generateerrorhttpresponse() {

	jimstring retrn;

	jimstring content("<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center></body></html>");

	retrn = "HTTP/1.1 400 Bad Request\r\n";
	retrn += "Content-Type: text/html\r\n";
	retrn += "Content-Length: " + _(content.len) + "\r\n";
	retrn += "Connection: close\r\n";
	retrn += "\r\n";
	retrn += content;

	return retrn;
}

jimstring SimpleWeb::urlencode(const char* data) {

	jimstring retrn;

	if (data) {

		jimstring text(data);

		retrn.resize(text.len * 3);

		int retrnpos = 0;

		for (int a = 0; a < text.len; a++) {

			unsigned char thechar = text[a];

			if (
				(thechar >= '0' && thechar <= '9')
				|| (thechar >= 'A' && thechar <= 'Z')
				|| (thechar >= 'a' && thechar <= 'z')
				|| thechar == '-'
				|| thechar == '_'
				|| thechar == '.'
				|| thechar == '~'
			) {

				retrn[retrnpos++] = thechar;
			}
			else {

				retrn[retrnpos++] = '%';
				retrn[retrnpos++] = simplestring.convertchartohex(thechar / 16);
				retrn[retrnpos++] = simplestring.convertchartohex(thechar % 16);
			}
		}

		retrn.truncate(retrnpos);
	}

	return retrn;
}

jimstring SimpleWeb::urldecode(const char* data) {

	jimstring retrn;

	if (data) {

		jimstring text(data);

		retrn.resize(text.len);

		int retrnpos = 0;

		for (int a = 0; a < text.len; a++) {

			unsigned char thechar = text[a];

			if (thechar == '%') {

				retrn[retrnpos++] = (simplestring.converthextochar(text[a + 1]) * 16) + simplestring.converthextochar(text[a + 2]);
				a += 2;
			}
			else {

				retrn[retrnpos++] = thechar;
			}
		}

		retrn.truncate(retrnpos);
	}

	return retrn;
}
