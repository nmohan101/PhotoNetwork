bool istest = true;

// SimpleCoding Wrapper
	#include "simpleinclude.h"

int listenport = 8553;

jimstring currentpath(simplefilefunctions.getcurrentpath());


void clientconnected(simpleclientsocket& clientsock);
jimstring applicationrequest(jimstring& resource, jimhash &fdat);


simpleserver theserver;

jimstring jsondata("{\"data\":\"Original Data\"}");

int main() {

	msgln("");

	if (!theserver.init($(
		"onclientaccepted", &clientconnected,
		"simplefullstack", true,
		"sharedstaticcontentfolder", _("/http/jslib"),
		"sharedstaticcontentfiles", jimvector<jimstring>({
			_("/simplejsutils_1.js"),
			_("/simpledomutils_1.js"),
			_("/simplefullstack_1.js")
		}),
		"staticcontentfolder", _("/http"),
		"staticcontentfiles", jimvector<jimstring>({
			_("/main_1.js")
		}),
		"onapplicationrequest", &applicationrequest
	))) {

		msgln("Could not initialize server");

		return 0;
	}

	if (!theserver.startssl(listenport, "cert.crt", "privatekey.key")) {

		msgln("Could not listen on port " + _(listenport));

		return 0;
	}

	msgln("Started server on port " + _(listenport));

	while (1) {

		theserver.serviceconnections();
	}

	return 0;
}

void clientconnected(simpleclientsocket& clientsock) {

}

jimstring applicationrequest(jimstring& resource, jimhash& request) {

	jimstring retrn;

	if (resource == "/getdata") {
		
		msgln("Front end requested data. Sending response now.");
		
		/////////////////////////////////////////
		// Put the code here to populate "jsondata"
		// with the json you would like to send to the browser (or maybe your code already did that, so just use the value)

		



		/////////////////////////////////////////

		retrn = simpleweb.generatevalidhttpresponse(jsondata);
	}
	else if (resource == "/buttonclicked") {
		
		msgln("Front end sent button click event. Processing now.");
		
		/////////////////////////////////////////
		// Put the code that should be executed when
		// the user clicks the button in the browser here.
		
		if (request.exists("data")) {
		
			jsondata = (jimstring)request["data"];
			
			msgln("received data: " + jsondata);
		}


		/////////////////////////////////////////
	}
	else {

		msgln("Front end sent unknown request.");
		
		
		
		msgln("Unknown resource in request: " + resource);

		retrn.fail();
	}

	return retrn;
}
