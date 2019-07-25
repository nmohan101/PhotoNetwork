class jimitem {
public:

	jimstring type;
	jimstring key;
	void* value;
	bool borrowed;
	bool failed;
//////////////////////////////////////
	void (*destructorcallback)(void*);
	void (*copycallback)(void*&, const void*);
/////////////////////////////////////////////

	void init();

	jimitem();
	template <class templatetype> jimitem(const char* newkey, const templatetype& valueref);
	jimitem(const jimitem& match);
	jimitem(jimitem&& match);
	explicit jimitem(const jimstring& newkey);
	explicit jimitem(jimstring&& newkey);
	explicit jimitem(const char* newkey);

	template <class templatetype> void setcallbacks(const templatetype* examplepointer);

	~jimitem();

	jimitem& destroy();

	jimitem give();

	const jimitem& fail();

	jimitem& copy(const jimitem& match);
	jimitem& steal(jimitem& match);
	jimitem& steal(jimitem&& match);

	jimitem& operator=(const jimitem& match);
	jimitem& operator=(jimitem&& match);
	template <class templatetype> jimitem& operator=(const templatetype& valueref);

	template <class templatetype> static jimstring gettypestring(const templatetype& thetype);
	template <class templatetype> jimitem& settypestring(const templatetype& thetype);

	//template <class templatetype> operator templatetype() const;
	template <class templatetype> operator templatetype&() const;

	jimitem& operator[](int theindex);
	const jimitem& operator[](int theindex) const;
	jimitem& operator[](const char* thekey);
	const jimitem& operator[](const char* thekey) const;
	jimitem& operator[](const jimstring& thekey);
	const jimitem& operator[](const jimstring& thekey) const;

	jimitem& index(int theindex);
	const jimitem& index(int theindex) const;
	jimitem& index(const char* thekey);
	const jimitem& index(const char* thekey) const;
	jimitem& index(const jimstring& thekey);
	const jimitem& index(const jimstring& thekey) const;
};

template <class templatetype> templatetype& operator<(templatetype& destination, const jimitem& source);
template <class templatetype> templatetype& operator<(templatetype& destination, jimitem&& source);