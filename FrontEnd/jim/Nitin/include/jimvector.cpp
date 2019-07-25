template <class type>
void jimvector<type>::init() {

	size = 0;
	array = 0;
	datasize = 0;
	borrowed = false;
	failed = false;
}

template <class type>
jimvector<type>::jimvector() {

	init();
}
template <class type>
jimvector<type>::jimvector(int size) {

	init();
	resize(size);
}
template <class type>
jimvector<type>::jimvector(const jimvector<type>& match) {

	init();
	copy(match.use(), match.size);
}
template <class type>
jimvector<type>::jimvector(jimvector<type>&& match) {

	init();
	steal(match);
}
template <class type>
template <std::size_t arraysize>
jimvector<type>::jimvector(const type (& match) [arraysize]) {

	init();
	copy(match, arraysize);
}

template <class type>
jimvector<type>::jimvector(const jimitem& match) {

	init();
	*this = *(jimvector<type>*)match.value;
}

template <class type>
jimvector<type>::~jimvector() {

	destroy();
}

template <class type>
void jimvector<type>::resize(int length) {

	debug.enter("jimvector", "jimvector::resize(int=", _(length).use(), ")");

	if (size && !borrowed) {

		delete []array;

	}

	size = length;
	datasize = size * sizeof(type);
	borrowed = false;
	failed = false;

	if (size) {

		array = new type[size];
	}

	debug.exit("jimvector", "jimvector::resize(int)");
}

template <class type>
void jimvector<type>::resize(int length, const type& match) {

	resize(length);

	for (int a = 0; a < size; a++) {

		index(a) = match;
	}
}

template <class type>
void jimvector<type>::destroy() {

	debug.enter("jimvector", "jimvector::destroy()");

	resize(0);

	debug.exit("jimvector", "jimvector::destroy()");
}

template <class type>
type* jimvector<type>::use(int elementindex) {

	return &index(elementindex);
}
template <class type>
type* jimvector<type>::use(int elementindex) const {

	return &index(elementindex);
}

template <class type>
jimvector<type> jimvector<type>::give() {

	jimvector<type> retrn(std::move(*this));
	return retrn;
}

template <class type>
jimvector<type>& jimvector<type>::fail() {

	destroy();
	failed = true;

	return *this;
}

template <class type>
template <class matchtype>
int jimvector<type>::find(const matchtype& match, int startpos) {

	for (int a = startpos; a < size; a++) {

		if (index(a) == match) {

			return a;
		}
	}

	return -1;
}

template <class type>
type* jimvector<type>::insertnum(int pos, int num) {

	if (num) {

		jimvector<type> retrn(size + num);

		for (int a = 0; a < pos; a++) {

			retrn[a] = std::move(index(a));
		}

		for (int a = pos; a < size; a++) {

			retrn[a + num] = std::move(index(a));
		}

		steal(retrn);
	}

	return use(pos);
}
template <class type>
type* jimvector<type>::insertnum(int pos, int num, const type& match) {

	insertnum(pos, num);

	for (int a = 0; a < num; a++) {

		index(pos + a) = match;
	}

	return use(pos);
}

template <class type>
void jimvector<type>::insert(int pos, const jimvector<type>& match) {

	insertnum(pos, match.size);

	for (int a = 0; a < match.size; a++) {

		index(a + pos) = match[a];
	}
}
template <class type>
void jimvector<type>::insert(int pos, jimvector<type>&& match) {

	insertnum(pos, match.size);

	for (int a = 0; a < match.size; a++) {

		index(a + pos) = std::move(match[a]);
	}
}
template <class type>
void jimvector<type>::insert(int pos, const type& match) {

	insertnum(pos, 1, match);
}
template <class type>
void jimvector<type>::insert(int pos, type&& match) {

	insertnum(pos, 1);
	index(pos) = std::move(match);
}

template <class type>
type* jimvector<type>::addnum(int num) {

	return insertnum(size, num);
}
template <class type>
type* jimvector<type>::addnum(int num, const type& match) {

	return insertnum(size, num, match);
}

template <class type>
void jimvector<type>::add(const jimvector<type>& match) {

	insert(size, match);
}
template <class type>
void jimvector<type>::add(jimvector<type>&& match) {

	insert(size, std::move(match));
}
template <class type>
void jimvector<type>::add(const type& match) {

	insert(size, match);
}
template <class type>
void jimvector<type>::add(type&& match) {

	insert(size, std::move(match));
}

template <class type>
jimvector<type>& jimvector<type>::del(int pos, int num) {

	if (!num) {

		return *this;
	}

	jimvector<type> retrn(size - num);

	if (pos < 0) {

		pos += size;
	}

	for (int a = 0; a < pos; a++) {

		retrn[a] = index(a);
	}

	for (int a = pos + num; a < size; a++) {

		retrn[a - num] = index(a);
	}

	return steal(retrn);
}

template <class type>
jimvector<type>& jimvector<type>::borrow(const type* match, int length) {

	destroy();
	array = match;
	size = length;
	datasize = size * sizeof(type);
	borrowed = true;

	return *this;
}

template <class type>
jimvector<type>& jimvector<type>::borrow(const jimvector<type>& match) {

	return borrow(match.use(), match.size);
}

template <class type>
jimvector<type>& jimvector<type>::copy(const type* match, int length) {

	resize(length);

	for (int a = 0; a < length; a++) {

		index(a) = match[a];
	}

	return *this;
}
template <class type>
jimvector<type>& jimvector<type>::copy(const jimvector<type>& match) {

	copy(match.array, match.size);

	failed = failed;

	return *this;
}

template <class type>
jimvector<type>& jimvector<type>::steal(jimvector<type>& match) {

	destroy();

	if (match.borrowed) {

		copy(match);
	}
	else {

		array = match.array;
		size = match.size;
		datasize = match.datasize;
		failed = failed;
	}

	match.init();

	return *this;
}

template <class type>
jimvector<type>& jimvector<type>::operator=(const jimvector<type>& match) {

	copy(match.use(), match.size);

	return *this;
}

template <class type>
jimvector<type>& jimvector<type>::operator=(jimvector<type>&& match) {

	steal(match);

	return *this;
}

template <class type>
jimvector<type>& jimvector<type>::operator+=(const type& match)
{
	addnum(1, match);

	return *this;
}
template <class type>
jimvector<type>& jimvector<type>::operator+=(type&& match) {

	insert(size, std::move(match));

	return *this;
}
template <class type>
jimvector<type>& jimvector<type>::operator+=(const jimvector<type>& match)
{
	insert(size, match);

	return *this;
}
template <class type>
jimvector<type>& jimvector<type>::operator+=(jimvector<type>&& match) {

	insert(size, std::move(match));

	return *this;
}

template <class type>
type& jimvector<type>::operator[](int theindex) {

	return index(theindex);
}

template <class type>
type& jimvector<type>::operator[](int theindex) const {

	return index(theindex);
}

template <class type>
type& jimvector<type>::index(int theindex) {

	if (theindex < 0) {

		theindex += size;
	}

	return array[theindex];
}

template <class type>
type& jimvector<type>::index(int theindex) const {

	return array[theindex];
}

template <class type>
int jimvector<type>::find(const type& match, int start) {

	for (int a = start; a < size; a++) {

		if (index(a) == match) {

			return a;
		}
	}
	return -1;
}

template <class type, class streamtype>
streamtype& operator>>(streamtype& theinfile, jimvector<type>& info)
{
	int size;

	theinfile >> size;

	if (theinfile.eof() == true) {

		size = 0;
	}

	info.resize(size);
	theinfile.read(info.use(), info.size);

	return theinfile;
};

template <class type, class streamtype>
streamtype& operator<<(streamtype& theoutfile, const jimvector<type>& info)
{

	theoutfile << info.size;
	theoutfile.write(info.use(), info.size);

	return theoutfile;
};
