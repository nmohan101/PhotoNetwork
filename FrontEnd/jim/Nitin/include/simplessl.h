class SimpleSSL {
public:

	bool islibraryinitialized;

	SSL_METHOD *clientmethod, *servermethod;
	SSL_CTX *clientcontext, *servercontext;

	SimpleSSL();

	bool initlibrary();
	bool initclientcontext();
	bool initservercontext();
};

SimpleSSL simplessl;