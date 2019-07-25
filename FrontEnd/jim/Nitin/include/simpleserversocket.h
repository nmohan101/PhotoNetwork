class simpleserversocket {
public:

	int thesocket;
	sockaddr_in thesockaddr;

	bool isssl;

    simpleserversocket();
	~simpleserversocket();

	operator int();

	bool loadcerts(const char* certfilename, const char* keyfilename);

	bool plaintextlisten(int serverport);
	bool ssllisten(int serverport);
	bool websocketlisten(int serverport);

	void disconnect();

	bool connectionwaiting();

	simpleclientsocket acceptconnection();
};

bool operator==(const simpleserversocket& sock1, const simpleserversocket& sock2);