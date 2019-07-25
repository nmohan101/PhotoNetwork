
class SimpleWeb {
public:

	jimhash request(simpleclientsocket& clientsocket, const char* host, const char* operation, const char* postdata, const jimhash& header);

	jimstring getipfromurl(const char* url);

	jimhash httpsrequest(const char* host, const char* operation, const char* postdata, const jimhash& header);

	jimhash parsehttprequest(simpleclientsocket& clientsocket);

	jimhash parsequerystring(const char* querystring);
	jimstring converttoquerystring(const jimhash& fdat);

	jimstring generatevalidhttpresponse(const char* data, const jimhash& header = jimhash());
	jimstring generateerrorhttpresponse();

	jimstring urlencode(const char* data);
	jimstring urldecode(const char* data);
};

SimpleWeb simpleweb;


