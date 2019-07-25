class simpleserver {
public:

	simpleserversocket serversocket;
	jimvector<simpleclientsocket> clientsockets;

	fd_set activesockets;
	int maxsocketid;

	jimhash users;

	// Args:
	void (*acceptcallback)(simpleclientsocket&);
	void (*receivemessagecallback)(simpleclientsocket&);
	bool simplefullstack;
	jimstring sharedstaticcontentfolder;
	jimstring staticcontentfolder;
	jimvector<jimstring> sharedstaticcontentfiles;
	jimvector<jimstring> staticcontentfiles;
	jimstring (*applicationrequestcallback)(jimstring&, jimhash&);

	simpleserver();
	simpleserver(const jimhash& serveroptions);
	//void (*theacceptcallback)(simpleclientsocket&), void (*thereceivemessagecallback)(simpleclientsocket&));
	bool init(const jimhash& serveroptions);

	bool startplaintext(int port);
	bool startssl(int port, const char* certfilename, const char* keyfilename);
	bool startwebsockets();

	void buildactivesocketfdlist();
	bool serviceconnections();
	void onmessagereceiveddefault(simpleclientsocket& clientsocket);

	bool adduser(const jimstring& username, const jimstring& password1, const jimstring& password2, jimstring& errorreturn);
	bool login(const jimstring& username, const jimstring& password, const jimstring& accesskey, jimstring& accesskeyreturn, jimstring& errorreturn);
};
