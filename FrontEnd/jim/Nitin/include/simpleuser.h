class simpleuser {
public:

	jimstring username;
	jimstring passwordmd5;
	jimstring accesskey;

	simpleuser();
	simpleuser(const char* username, const char* thepassword);

	bool login(const jimstring& password, const jimstring& accesskey, jimstring& accesskeyreturn, jimstring& errorreturn);
};

//Stream Overload
template <class type>
type& operator>>(type& stream, simpleuser& info);

template <class type>
type& operator<<(type& stream, const simpleuser& info);