class simpledebug {
public:

	int tabcount;
	bool debugging;
	jimstring types;

	simpledebug();

	template<class... args> void msgrecurse(const char* text);
	template<class... args> void msgrecurse(const char* text, args... myargs);

	template<class... args> void debugmsg(args... myargs);

	template<class... args> void enter(const char* type, args... myargs);
	template<class... args> void exit(const char* type, args... myargs);
	template<class... args> void fail(const char* type, args... myargs);
	template<class... args> void print(const char* type, args... myargs);
};