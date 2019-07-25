class jimitem;

void jimstring::init() {

	len = 0;
	array = 0;
	borrowed = false;
	failed = false;
}
jimstring::jimstring() {

	init();
}
jimstring::jimstring(int length, char* match) {

	init();
	resize(length, match);
}
jimstring::jimstring(const char* initvalue) {

	init();
	copy(initvalue, (int)strlen(initvalue));
}
jimstring::jimstring(const jimstring& initvalue) {

	debug.enter("jimstring", "jimstring::jimstring(const jimstring&");

	init();
	copy(initvalue);

	debug.exit("jimstring", "jimstring::jimstring(const jimstring&");
}
jimstring::jimstring(jimstring&& initvalue) {


	debug.enter("jimstring", "jimstring::jimstring(jimstring&&");

	init();
	steal(initvalue);


	debug.exit("jimstring", "jimstring::jimstring(jimstring&&");
}
jimstring::jimstring(const jimitem& match) {

	init();
	*this = *(jimstring*)match.value;
}

jimstring::~jimstring() {

	destroy();
}

int jimstring::resize(int length, char* match) {

	if (len && !borrowed) {

		debug.print("jimstring", "jimstring::destroy() ", array);

		delete []array;
	}

	len = length;

	if (len) {

		array = new char[len + 1];
		array[len] = 0;

		if (match) {

			jimstring repeat;

			repeat.borrow(match);

			if (repeat.len) {

				int repeatpos = 0;
				int repeatsize;

				while (repeatpos < len) {

					repeatsize = repeat.len;

					if (repeatpos + repeat.len > len) {

						repeatsize = len - repeatpos;
					}

					memcpy((void*)use(repeatpos), (void*)repeat.use(), repeatsize);

					repeatpos += repeatsize;
				}
			}
		}
	}
	else {

		array = 0;
	}

	borrowed = false;
	failed = false;

	return length;
}

void jimstring::destroy() {

	resize(0);
}

char* jimstring::use(int elementindex) {

	return &array[elementindex];
}
char* jimstring::use(int elementindex) const {

	return &array[elementindex];
}

jimstring jimstring::give() {

	debug.enter("jimstring", "jimstring::give()");


	jimstring retrn(std::move(*this));


	debug.exit("jimstring", "jimstring::give()");


	return retrn;
}

jimstring& jimstring::fail() {

	destroy();
	failed = true;
	return *this;
}

jimstring& jimstring::truncate(int newlength = -1) {

	if (newlength == -1) {

		len = (int)strlen(array);
	}
	else if (!newlength) {

		destroy();
	}
	else {

		len = newlength;
		array[len] = 0;
	}

	return *this;
}

jimstring jimstring::seltext(int pos, int length) const {

	jimstring retrn(length);

	if (pos < 0) {

		pos += len;
	}

	memcpy((void*)retrn.array, (void*)&array[pos], length);

	return retrn;
}
jimstring jimstring::seltext(int pos) const {

	return seltext(pos, len - pos);
}

int jimstring::find(const char* match, int start) const {

	return findandmove(match, start);
}
int jimstring::find(const jimstring& match, int start) const {

	return findandmove(match, start);
}
int jimstring::findandmove(const char* match, int& start) const {

	jimstring matchholder;
	matchholder.borrow(match);

    return findandmove(matchholder, start);
}
int jimstring::findandmove(const jimstring& match, int& start) const {

    int retrn, b;

	while (start <= len - match.len) {

		for (b = 0; b < match.len; b++) {

			if (array[start + b] != match[b]) {

				break;
			}
			else if (b == match.len - 1) {

				retrn = start;
				start += match.len;

				return retrn;
			}
		}

		start++;
	}

	start = len;

	return -1;
}

jimstring jimstring::replace(const char* thefindstring, const char* thereplacestring) const {

	jimstring findstring, replacestring;
	findstring.borrow(thefindstring);
	replacestring.borrow(thereplacestring);

	return replace(findstring, replacestring);
}

jimstring jimstring::replace(const jimstring& findstring, const jimstring& replacestring) const {

	int position = 0;

	int replacecount = 0;

	// Get # of replacements soo that we can preclaculate new len
	while (findandmove(findstring, position) > -1) {

		replacecount++;
	}

	if (!replacecount) {

		return *this;
	}

	// There are matches to replace

	jimstring retrn(len + (replacecount * (replacestring.len - findstring.len)));

	position = 0;
	int findposition;
	int writeposition = 0;

	// Keep finding matches against findstring, until there are none left
	while ((findposition = find(findstring, position)) > -1) {

		int beforestringsize = findposition - position;

		// Copy the unmatched string memory, if there is any
		if (beforestringsize) {

			memcpy(retrn.use(writeposition), use(position), beforestringsize);
			writeposition += beforestringsize;
		}

		// Copy the replacement value, in place of of the matche
		memcpy(retrn.use(writeposition), replacestring.use(), replacestring.len);
		writeposition += replacestring.len;

		// Advance the current position to beyond the matche
		position = findposition + findstring.len;
	}

	int afterstringsize = len - position;

	// Copy the final unmatched string memory, if there is any
	if (afterstringsize) {

		memcpy(retrn.use(writeposition), use(position), afterstringsize);
	}

	return retrn;
}

jimstring jimstring::getupto(const char *match, int& pos) const {

	jimstring retrn;

	int origpos = pos;
	int found = -1;

	if (match && pos > -1) {

		found = findandmove(match, pos);
	}

	if (found == -1) {

		pos = origpos;

		retrn.fail();

		return retrn;
	}

    return seltext(origpos, found - origpos);
}

jimvector<jimstring> jimstring::split(const char* match) const {

	jimvector<jimstring> retrn;
	int pos = 0;
	jimstring itemstring;

	while (1) {

		itemstring = getupto(match, pos);

		if (itemstring.failed) {

			retrn += seltext(pos);

			break;
		}

		retrn += itemstring;
	}

	return retrn;
}

jimstring& jimstring::borrow(const char* match, int length) {

	debug.enter("jimstring", "jimstring::borrow(char*=", match, ", int=", _(length).use(), ")");

	destroy();
	array = (char*)match;
	len = length;
	borrowed = true;

	debug.exit("jimstring", "jimstring::borrow(char*, int)");

	return *this;
}
jimstring& jimstring::borrow(const char* match) {

	return borrow(match, (int)strlen(match));
}
jimstring& jimstring::borrow(const jimstring& match) {

	return borrow(match.array, match.len);
}

jimstring& jimstring::copy(const char* match, int length) {

	debug.print("jimstring", "jimstring::copy(const char*, int) - \"", match, "\"");

	resize(length);

	if (length) {

		memcpy((void*)array, (void*)match, length);
	}

	return *this;
}
jimstring& jimstring::copy(const jimstring& match) {

	copy(match.use(), match.len);

	failed = match.failed;

	return *this;
}

jimstring& jimstring::steal(jimstring& other) {

	debug.print("jimstring", "jimstring::steal(const char*, int) - \"", other.use(), "\"");

	destroy();

	if (other.borrowed) {

		copy(other);
	}
	else {

		array = other.array;
		len = other.len;
		failed = other.failed;
	}

	other.init();

	return *this;
}

/*jimstring& jimstring::steal(jimstring&& other) {

	return steal(other);
}*/

jimstring& jimstring::operator=(const char *match) {

	if (strlen(match)) {

		copy(match, (int)strlen(match));
	}
	else {

		destroy();
	}

	return *this;
}
jimstring& jimstring::operator=(const jimstring& match) {


	debug.enter("jimstring", "jimstring::operator=(const jimstring&)");

	copy(match);

	debug.exit("jimstring", "jimstring::operator=(const jimstring&)");

	return *this;
}
jimstring& jimstring::operator=(jimstring&& match) {

	debug.enter("jimstring", "jimstring::operator=(jimstring&&)");

	steal(match);

	debug.exit("jimstring", "jimstring::operator=(jimstring&&)");

	return *this;
}
jimstring& jimstring::operator+=(const char* match) {

	return *this = *this + match;
}
jimstring& jimstring::operator+=(const jimstring& match) {

	return *this = *this + match;
}

bool jimstring::isint() const {

	for (int a = 0; a < len; a++) {

		char &currentchar = array[a];

		if (
			(
				currentchar != '-'
				|| a
			)
			&& (
				currentchar < 48
				|| currentchar > 57
			)
		) {

			return false;
		}
	}

	return true;
}
int jimstring::toint() const {

	return atoi(use());
}
float jimstring::tofloat() const {

	return (float)atof(use());
}
double jimstring::todouble() const {

	return atof(use());
}
jimstring jimstring::md5() const {

	jimstring retrn(MD5_DIGEST_LENGTH);

	MD5(*this, len, retrn);

	return retrn;
}

bool jimstring::regexmatch(const char* regex) const {

	return (
		len
		&& std::regex_match(use(), std::regex(regex))
	);
}

jimstring jimstring::regexreplace(const char* match, const char* replacewith, const char* theflags = 0) const {

	jimstring flags;

	if (theflags) {

		flags.borrow(theflags);
	}

	std::regex_constants::match_flag_type regexflags(std::regex_constants::match_default);

	if (flags.find("g") == -1) {

		regexflags = std::regex_constants::format_first_only;
	}

	return _(std::regex_replace(use(), std::regex(match), replacewith, regexflags).c_str());
}

char& jimstring::operator[](int indx) {

	if (indx < 0) {

		return array[len + indx];
	}
	else {

		return array[indx];
	}
}
char& jimstring::operator[](int indx) const {

	if (indx < 0) {

		return array[len + indx];
	}
	else {

		return array[indx];
	}
}

bool operator==(const jimstring& thing, const char* match) {

	if (thing.len == 0 && strlen(match) == 0) {

		return true;
	}
	else if (thing.len == 0 && strlen(match) > 0) {

		return false;
	}

	return !strcmp(thing.array, match);
}
bool operator==(const char* match, const jimstring& thing) {

	return thing == match;
}
bool operator==(const jimstring& thing, const jimstring& match) {

	bool retrn = true;

	if (thing.len != match.len) {

		return false;
	}
	else {

		for (int a = 0; a < thing.len; a++) {

			if (thing[a] != match[a]) {

				return false;
			}
		}
	}

	return true;
}
bool operator!=(const jimstring& thing, const char* match) {

	return !(thing == match);
}
bool operator!=(const char* match, const jimstring& thing) {

	return !(thing == match);
}
bool operator!=(const jimstring& thing, const jimstring& match) {

	return !(thing == match);
}

jimstring concat(const char* firstarray, int firstlength, const char* secondarray, int secondlength) {

	jimstring retrn(firstlength + secondlength);

	memcpy((void*)retrn.use(), (void*)firstarray, firstlength);
	memcpy((void*)retrn.use(firstlength), (void*)secondarray, secondlength);

	return retrn;
}

jimstring operator+(const jimstring& first, const jimstring& second) {

	return concat(first.use(), first.len, second.use(), second.len);
}
jimstring operator+(const jimstring& first, const char* second) {

	return concat(first.use(), first.len, second, (int)strlen(second));
}
jimstring operator+(const char* first, const jimstring& second) {

	return concat(first, (int)strlen(first), second.use(), second.len);
}

jimstring _(const char* string, int bytenum) {

	if (bytenum == -1) {

		bytenum = (int)strlen(string);
	}

	jimstring retrn(bytenum);

	memcpy(retrn.use(), string, bytenum);

	return retrn;
}
jimstring _(int theint) {

    jimstring retrn(20);

	snprintf(retrn.use(), retrn.len, "%d", theint);
    //_itoa_s(theint, retrn.use(), 20, 10);
	retrn.truncate();

    return retrn;
}
jimstring _(__int64 theint) {

    jimstring retrn(20);

	snprintf(retrn.use(), retrn.len, "%llu", (long long unsigned int)theint);
    //_i64toa_s(theint, retrn.use(), 20, 10);
	retrn.truncate();

    return retrn;
}
jimstring _(double num) {

    jimstring retrn(50);

	snprintf(retrn.use(), retrn.len, "%f", num);
    //_gcvt_s(retrn.use(), retrn.len, num, precision);
	retrn.truncate();

	if (retrn[-1] == '.') {

		retrn[--retrn.len] = 0;
	}

    return retrn;
}
char* _(const jimstring& string) {

	return string.use();
}

jimstring __(int theint) {

	return _((char*)&theint, 4);
}
jimstring __(const jimstring& thestring) {

	jimstring retrn;

	retrn.resize(4 + thestring.len);
	memcpy((void*)retrn.use(), (void*)&thestring.len, 4);
	memcpy((void*)retrn.use(4), (void*)thestring.use(), thestring.len);

	return retrn;
}

template <class datatype> bool __(datatype& stream, jimstring& string) {

	//ToDo: This should be changed to use failed and just return the jimstring
	int size;

	if (!stream.read(&size, 4)) {

		return false;
	}

	string.resize(size);

	if (!stream.read(string.use(), string.len)) {

		return false;
	}

	return true;
}

void msg(const jimstring& message) {

	msg(message.use());
}
void msgln(const jimstring& message) {

	msg((message + "\n").use());
}

//Stream Overload
template <class type>
type& operator>>(type& stream, jimstring& info) {
	int size;

	stream >> size;

	if (stream.eof() == true) {

		info.fail();
	}
	else {

		info.resize(size);

		if (size) {

			stream.read(info.use(), size);
		}
	}

	return stream;
}

template <class type>
type& operator<<(type& stream, const jimstring& info) {

	stream << info.len;
	stream.write(info.use(), info.len);

	return stream;
}
