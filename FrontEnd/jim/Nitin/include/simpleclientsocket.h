class simpleclientsocket {
public:

    SOCKET thesocket;
    SSL *sslconnection;
    sockaddr_in thesockaddr;

	simpleclientsocket();
	simpleclientsocket(int newsocket);
	void init();

	operator int();

	bool plaintextconnect(const char* ipadress, int serverport);
	bool sslconnect(const char* ipadress, int serverport);
	bool websocketconnect(const char* ipadress, int serverport);

	bool isconnected();

	void disconnect();

	jimstring getserversslcertificate();

	bool sendmessage(const char* text, int length);
	bool sendmessage(const jimstring& text);

	jimstring getmessage(int maxsize);
	jimstring getmessagesmart();

	jimstring getip();
	jimstring getipstring();
};

bool operator==(const simpleclientsocket& sock1, const simpleclientsocket& sock2);

