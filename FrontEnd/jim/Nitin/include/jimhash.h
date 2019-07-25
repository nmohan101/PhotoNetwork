

//ToDo: Add all the ifndefs for the h files to do it right
//ToDo: make sure that
class jimhash {
public:

	int size;
	jimvector<jimvector<jimitem> >* buckets;
	bool failed;
	bool borrowed;

	jimitem faildummy;

	void init();

	jimhash();
	jimhash(const jimhash& match);
	jimhash(jimhash&& match);
//ToDo: make these template, arglist functions should just be the face, and then the worhorse should count how many elements using recursion and then it can properly size on the first time, no matter how many elements are built in. perfect for parameters. pass a couting integer by reference and increments it and the last function actually does the resizing.
	template<class templatetype, class... args> jimhash(const char* thekey, const templatetype& thevalue, args... myargs);
	template<class... args> jimhash(const char* thekey, const char* thevalue, args... myargs);

	~jimhash();

	void destroy();

	jimhash give();

	jimhash& fail();

	template<class... args> jimhash& copyitems(const jimitem& theitem, args... myargs);
	jimhash& copyitem(const jimitem& theitem);
	template<class templatetype, class... args> jimhash& copyitems(const char* thekey, const templatetype& thevalue, args... myargs);
	template<class templatetype> jimhash& copyitem(const char* thekey, const templatetype& thevalue);
	jimhash& copyitems();

	template<class... args> jimhash& stealitems(jimitem& theitem, args... myargs);
	jimhash& stealitem(jimitem& theitem);
	template<class... args> jimhash& stealitems(jimitem&& theitem, args... myargs);
	jimhash& stealitem(jimitem&& theitem);

	jimhash& operator=(const jimhash& match);
	jimhash& operator=(jimhash&& match);

	jimhash& copy(const jimhash& match);
	jimhash& steal(jimhash& match);
	jimhash& steal(jimhash&& match);

	jimitem& operator[](const char* key);
	const jimitem& operator[](const char* key) const;
	jimitem& operator[](const jimstring& key);
	const jimitem& operator[](const jimstring& key) const;
	jimitem& operator[](int index);
	const jimitem& operator[](int index) const;

	jimitem& index(const char* key);
	const jimitem& index(const char* key) const;
	jimitem& index(const jimstring& key);
	const jimitem& index(const jimstring& key) const;
	jimitem& index(int theindex);
	const jimitem& index(int theindex) const;

	bool exists(const char* key) const;
	template<class type> type& ensure(const char* key, const type& example);
	bool del(const char* key);
	// Make versions of the funcitons that let you do multiple things at once and return a jimvector of return results

	int finditemindex(jimvector<jimitem>* bucket, const char* key) const;

	void mayberestructure();

	int computebucketindex(const char* key, int bucketsize = -1) const;

	jimvector<jimstring> getkeys() const;

	template<class type>
	bool params(type& variableref, const char* paramstring) const;

	template<class type, class... args>
	bool params(type& variableref, const char* paramstring, args&... myargs) const;

	template <class streamtype, typename datatype>
	void save(streamtype& stream, const datatype& example) const;

	template <class streamtype, typename datatype>
	void load(streamtype& stream, const datatype& example);
};




