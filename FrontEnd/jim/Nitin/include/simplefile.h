class simplefile {
public:

	FILE* thestream;
	int matrixwidth, matrixheight, matrixunitsize, matrixheadersize, matrixpaddingmodulus;
	bool matrixinverty;

	simplefile();

	bool openforread(const char* path);
	bool openforwrite(const char* path);
	bool openforboth(const char* path);
	bool openfreshforboth(const char* path);

	template <class type> static bool save(const char* filename, const type& info);
	template <class type> static bool save(const char* filename, const jimhash& info, const type& stupidtype);
	template <class type> static bool load(const char* filename, type& info);
	template <class type> static bool load(const char* filename, jimhash& info, const type& stupidtype);

	bool isopen();
	bool eof();
    void close();

	void flush();
	template <class type> bool write(type* info, int size);
	template <class type> bool write(type& info);
	template <class type> bool read(type* info, int size);
	template <class type> bool read(type& info);
	//stream apply this to jimstrings (mke sure that order of operations doesn't fuck with shit
	//	example simplefile << jimstring << int
	//		that doesn't mean stream the int to the jimstring
	//		and then stream the jimstring into the file.
	// also make sure to use position just like  a file stream would. need to be able to overwrite
	// and resize if necessary when implemented on jimstring
	// instead of implementing them separately, bring jimstring in line so that it works with read and write
	// and then make this a multi-type template, the stream = type1 and the item = type2
	simplefile& operator<<(char* info);
	simplefile& operator<<(char info);
	simplefile& operator>>(char& info);
	simplefile& operator<<(unsigned char info);
	simplefile& operator>>(unsigned char& info);
	simplefile& operator<<(int info);
	simplefile& operator>>(int& info);
	simplefile& operator<<(float info);
	simplefile& operator>>(float& info);
	simplefile& operator<<(double info);
	simplefile& operator>>(double& info);
	simplefile& operator<<(__int64 info);
	simplefile& operator>>(__int64& info);

    __int64 getpos();
    void setpos(const __int64& newpos);
    void move(const __int64& num);
    void tostart();
    void toend();
	void ensuresize(const __int64& size);

	void matrixheaderend(int width, int height, int unitsize, bool inverty, int paddingmodulus);
	template<class type> bool matrixwrite(int x, int y, type* data, int size);
	template<class type> bool matrixread(int x, int y, type* data, int size);
};
