class jimitem;
template <class type> class jimvector;

class jimstring {
public:
	char* array;
	//ToDo: Add the idea of datasize, that way, len can be used like the current position as += is used. new data that gets added represents growth using "e" (or just doulbe the total of what we have so far.
	int len;
	bool borrowed;
	bool failed;

	void init();
	jimstring();
	explicit jimstring(int length, char* match = 0);
	explicit jimstring(const char* init);
	jimstring(const jimstring& init);
	jimstring(jimstring&& init);
	explicit jimstring(const jimitem& match);

	~jimstring();

	int resize(int length, char* match = 0);

	void destroy();

	char* use(int index = 0);
	char* use(int index = 0) const;

	jimstring give();

	jimstring& fail();

	jimstring& truncate(int newlength);

	jimstring seltext(int pos, int length) const;
	jimstring seltext(int pos) const;

	int find(const char* match, int start = 0) const;
	int find(const jimstring& match, int start = 0) const;
	int findandmove(const char* match, int& start) const;
	int findandmove(const jimstring& match, int& start) const;

	jimstring replace(const char* find, const char* replace) const;
	jimstring replace(const jimstring& findstring, const jimstring& replacestring) const;
	//ToDo: should have replaceandmove with char*
	//ToDo: should have replaceandmove with jimstring


	jimstring getupto(const char* match, int& pos) const;

	jimvector<jimstring> split(const char* match) const;

	jimstring& borrow(const char* match, int length);
	jimstring& borrow(const char* match);
	jimstring& borrow(const jimstring& match);

	jimstring& copy(const char* match, int length);
	jimstring& copy(const jimstring& match);

	jimstring& steal(jimstring& match);
	//jimstring& steal(jimstring&& match);

	jimstring& operator=(const char* match);
	jimstring& operator=(const jimstring& match);
	jimstring& operator=(jimstring&& match);
	jimstring& operator+=(const char* match);
	jimstring& operator+=(const jimstring& match);

	bool isint() const;
	int toint() const;
	float tofloat() const;
	double todouble() const;
	jimstring md5() const;

	bool regexmatch(const char* regex) const;
	jimstring regexreplace(const char* match, const char* replacewith, const char* theflags) const;

	char& operator[](int indx);
	char& operator[](int indx) const;

	operator char*() const {

		return use();
	};
	operator unsigned char*() const {

		return (unsigned char*)use();
	};
	//May need this. if you, you would probably have realized by now
	/*
	operator const char*() const {

		return use();
	};
	*/

	//operator jimitem();
};
bool operator==(const jimstring& thing, const char* match);
bool operator==(const char* match, const jimstring& thing);
bool operator==(const jimstring& thing, const jimstring& match);
bool operator!=(const jimstring& thing, const char* match);
bool operator!=(const char* match, const jimstring& thing);
bool operator!=(const jimstring& thing, const jimstring& match);

jimstring concat(const char* firstarray, int firstlength, const char* secondarray, int secondlength);
jimstring operator+(const jimstring& thing, const char* match);
jimstring operator+(const char* match, const jimstring& thing);
jimstring operator+(const jimstring& thing, const jimstring& match);

// Convert to string
jimstring _(const char* string, int bytenum = -1);
jimstring _(int theint);
jimstring _(__int64 theint);
jimstring _(double num);
char* _(const jimstring& string);

// Convert to stream
jimstring __(int theint);
jimstring __(const jimstring& thestring);

// Get from stream
template <class datatype> bool __(datatype& stream, jimstring& string);

// Msg
void msg(const jimstring& message);
void msgln(const jimstring& message);

template <class type> type& operator>>(type& stream, jimstring& info);
template <class type> type& operator<<(type& stream, const jimstring& info);
