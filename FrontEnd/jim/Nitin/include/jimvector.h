template <class type>
class jimvector {
public:
	int size;
	type* array;

	int datasize;
	bool borrowed;
	bool failed;

	void init();
	// ToDo:: add constructor and unpacks and gets size before it needs to assign a size and the snap the stack and place all the new elements
	jimvector();
	explicit jimvector(int size);
	jimvector(const jimvector<type>& match);
	jimvector(jimvector<type>&& match);

	// This allows jimvector<jimstring>({_("jim"), _("bob")})
	//ToDo then use yet another template variable so that you can do this with an array of any type that can convert to the main template type:"type"
	//	like this jimvector<jimstring>({"jim", "bob"}) (WHICH WOULD BE AWESOME)
	//ToDo aso write a packed args version of _() so that you can just do _("jim", "bob") if the way below turns out to be too big of a pain in the ass
	template <std::size_t arraysize>
	explicit jimvector(const type (& match) [arraysize]);

	// Needed to peroply do this:
	// 		jimvector thing = (jimvector<jimitem>)myjimitem
	// Otherwise, the compiler tries to autocast the jimitem to any type that works with a
	// jimvector contructor, but that is ambiguous because jimitem can be casted to anything.
	explicit jimvector(const jimitem& match);

	~jimvector();

	void resize(int length);
	void resize(int length, const type& match);

	void destroy();

	type* use(int index = 0);
	type* use(int index = 0) const;

	jimvector<type> give();

	jimvector<type>& fail();

	template <class matchtype> int find(const matchtype& match, int startpos = 0);

	type* insertnum(int pos, int num);
	type* insertnum(int pos, int num, const type& match);

	void insert(int pos, const jimvector<type>& match);
	void insert(int pos, jimvector<type>&& match);
	void insert(int pos, const type& match);
	void insert(int pos, type&& match);

	type* addnum(int num);
	type* addnum(int num, const type& match);

	void add(const jimvector<type>& match);
	void add(jimvector<type>&& match);
	void add(const type& match);
	void add(type&& match);

	jimvector<type>& del(int pos, int number = 1);

	jimvector<type>& borrow(const type* match, int length);
	jimvector<type>& borrow(const jimvector<type>& match);

	jimvector<type>& copy(const type* match, int length);
	jimvector<type>& copy(const jimvector<type>& match);

	jimvector<type>& steal(jimvector<type>& match);

	jimvector<type>& operator=(const jimvector<type>& match);
	jimvector<type>& operator=(jimvector<type>&& match);

	jimvector<type>& operator+=(const type& match);
	jimvector<type>& operator+=(type&& match);
	jimvector<type>& operator+=(const jimvector<type>& match);
	jimvector<type>& operator+=(jimvector<type>&& match);

	type& operator[](int index);
	type& operator[](int index) const;

	type& index(int index);
	type& index(int index) const;

	int find(const type& match, int start = 0);
};
