void jimhash::init() {

	debug.enter("jimhash", "void jimitem::init()");

	size = 0;
	buckets = 0;
	failed = false;
	borrowed = false;

	faildummy.fail();

	mayberestructure();

	debug.exit("jimhash", "void jimitem::init()");
}

jimhash::jimhash() {

	debug.enter("jimhash", "jimhash::jimhash()");

	init();

	debug.exit("jimhash", "jimhash::jimhash()");
}

jimhash::jimhash(const jimhash& match) {

	debug.enter("jimhash", "jimhash::jimhash(const jimhash&)");

	init();
	copy(match);

	debug.exit("jimhash", "jimhash::jimhash(const jimhash&)");
}
jimhash::jimhash(jimhash&& match) {

	debug.enter("jimhash", "jimhash::jimhash(jimhash&&)");

	init();
	steal(match);

	debug.exit("jimhash", "jimhash::jimhash(jimhash&&)");
}
template<class templatetype, class... args>
jimhash::jimhash(const char* thekey, const templatetype& thevalue, args... myargs) {

	debug.enter("jimhash", "jimhash::jimhash(const char*, const templatetype&, args...)");

	init();
	copyitems(thekey, thevalue, myargs...);

	debug.exit("jimhash", "jimhash::jimhash(const char*, const templatetype&, args...)");
}
template<class... args>
jimhash::jimhash(const char* thekey, const char* thevalue, args... myargs) {

	debug.enter("jimhash", "jimhash::jimhash(const char*, const char*, args...)");

	init();

	jimstring jimstringvalue = thevalue;

	copyitems(thekey, jimstringvalue, myargs...);

	debug.exit("jimhash", "jimhash::jimhash(const char*, const char*, args...)");
}


jimhash::~jimhash() {

	debug.enter("jimhash", "jimhash::~jimhash()");

	destroy();

	debug.exit("jimhash", "jimhash::~jimhash()");
}

void jimhash::destroy() {

	debug.enter("jimhash", "void jimhash::destroy()");

	if (buckets && !borrowed) {

		debug.print("jimhash", "Memory freed");

		delete buckets;
	}

	init();

	debug.exit("jimhash", "void jimhash::destroy()");
}

jimhash jimhash::give() {

	debug.enter("jimhash", "jimhash jimhash::give()");

	jimhash retrn(std::move(*this));

	debug.exit("jimhash", "jimhash jimhash::give()");

	return retrn;
}

jimhash& jimhash::fail() {

	debug.enter("jimhash", "jimhash& jimhash::fail()");

	destroy();
	failed = true;

	debug.exit("jimhash", "jimhash& jimhash::fail()");

	return *this;
}

template<class... args>
jimhash& jimhash::copyitems(const jimitem& theitem, args... myargs) {

	debug.enter("jimhash", "jimhash& jimhash::copyitems(const jimitem&, args...)");

	copyitem(theitem);
	copyitems(myargs...);

	debug.exit("jimhash", "jimhash& jimhash::copyitems(const jimitem&, args...)");

	return *this;
}
jimhash& jimhash::copyitem(const jimitem& matchitem) {

	debug.enter("jimhash", "jimhash& jimhash::copyitem(const jimitem&)");

	index(matchitem.key.use()).copy(matchitem);

	debug.exit("jimhash", "jimhash& jimhash::copyitem(const jimitem&)");

	return *this;
}
template<class templatetype, class... args>
jimhash& jimhash::copyitems(const char* thekey, const templatetype& thevalue, args... myargs) {

	debug.enter("jimhash", "jimhash& jimhash::copyitems(const char*, const templatetype&, args...)");

	copyitem(thekey, thevalue);
	copyitems(myargs...);

	debug.exit("jimhash", "jimhash& jimhash::copyitems(const char*, const templatetype&, args...)");

	return *this;
}
template<class templatetype>
jimhash& jimhash::copyitem(const char* thekey, const templatetype& thevalue) {

	debug.enter("jimhash", "jimhash& jimhash::copyitem(const char*, const templatetype&)");

	jimitem matchitem(thekey, thevalue);
	stealitem(matchitem);

	debug.exit("jimhash", "jimhash& jimhash::copyitem(const char*, const templatetype&)");

	return *this;
}
jimhash& jimhash::copyitems() {

	return *this;
}

template<class... args>
jimhash& jimhash::stealitems(jimitem& theitem, args... myargs) {

	debug.enter("jimhash", "jimhash& jimhash::stealitem(jimitem&, args...)");

	stealitem(theitem);
	stealitems(myargs...);

	debug.exit("jimhash", "jimhash& jimhash::stealitem(jimitem&, args...)");

	return *this;
}
jimhash& jimhash::stealitem(jimitem& newitem) {

	debug.enter("jimhash", "jimhash& jimhash::stealitem(jimitem&)");

	// ToDo: Is this weirdness necessary?
	jimitem& theitem = index(newitem.key.use());

	theitem.steal(newitem);

	debug.exit("jimhash", "jimhash& jimhash::stealitem(jimitem&)");

	return *this;
}
template<class... args>
jimhash& jimhash::stealitems(jimitem&& theitem, args... myargs) {

	debug.enter("jimhash", "jimhash& jimhash::stealitems(jimitem&&, args...)");

	stealitem(theitem);
	stealitems(myargs...);

	debug.exit("jimhash", "jimhash& jimhash::stealitems(jimitem&&, args...)");

	return *this;
}
jimhash& jimhash::stealitem(jimitem&& newitem) {

	debug.enter("jimhash", "jimhash& jimhash::stealitem(jimitem&&)");

	stealitem(newitem);

	debug.exit("jimhash", "jimhash& jimhash::stealitem(jimitem&&)");

	return *this;
}

/* doesn't make sense
template<class templatetype, class... args> jimhash& stealitem(const char* thekey, const templatetype& thevalue, args... myargs);
	template<class templatetype> jimhash& stealitem(const char* thekey, const templatetype& thevalue);
*/

jimhash& jimhash::operator=(const jimhash& match) {

	debug.enter("jimhash", "jimhash& jimhash::operator=(const jimhash&)");

	jimhash &retrn = copy(match);

	debug.exit("jimhash", "jimhash& jimhash::operator=(const jimhash&)");

	return retrn;
}
jimhash& jimhash::operator=(jimhash&& match) {

	debug.enter("jimhash", "jimhash& jimhash::operator=(jimhash&&)");

	jimhash &retrn = steal(match);

	debug.exit("jimhash", "jimhash& jimhash::operator=(jimhash&&)");

	return retrn;
}

jimhash& jimhash::copy(const jimhash& match) {

	debug.enter("jimhash", "jimhash& jimhash::copy(const jimhash&)");

	destroy();
	size = match.size;

	if (match.buckets) {

		jimvector<jimvector<jimitem> > &matchbuckets = *match.buckets;

		buckets = new jimvector<jimvector<jimitem> >(matchbuckets.size);

		jimvector<jimvector<jimitem> > &mybuckets = *buckets;

		for (int a = 0; a < matchbuckets.size; a++) {

			jimvector<jimitem> &matchbucket = matchbuckets[a];
			jimvector<jimitem> &mybucket = mybuckets[a];
			mybucket.resize(matchbucket.size);

			for (int b = 0; b < matchbucket.size; b++) {

				mybucket[b] = matchbucket[b];
			}
		}
	}

	debug.exit("jimhash", "jimhash& jimhash::copy(const jimhash&)");

	return *this;
}
jimhash& jimhash::steal(jimhash& match) {

	debug.enter("jimhash", "jimhash& jimhash::steal(jimhash&)");

	destroy();
	size = match.size;
	buckets = match.buckets;
	failed = match.failed;
	borrowed = match.borrowed; //ToDo: Should you be able to steal from a borrowed item? (Copy but throw runtime warning)
	match.init();

	debug.exit("jimhash", "jimhash& jimhash::steal(jimhash&)");

	return *this;
}
jimhash& jimhash::steal(jimhash&& match) {

	debug.enter("jimhash", "jimhash& jimhash::steal(jimhash&&)");

	steal(match);

	debug.exit("jimhash", "jimhash& jimhash::steal(jimhash&&)");

	return *this;
}

jimitem& jimhash::operator[](const char* key) {

	debug.enter("jimhash", "jimitem& jimhash::operator[](const char*)");

	jimitem &retrn = index(key);

	debug.exit("jimhash", "jimitem& jimhash::operator[](const char*) ", retrn.key.use());

	return retrn;
}
const jimitem& jimhash::operator[](const char* key) const {

	debug.enter("jimhash", "const jimitem& jimhash::operator[](const char*) const");

	const jimitem &retrn = index(key);

	debug.exit("jimhash", "const jimitem& jimhash::operator[](const char*) const");

	return retrn;
}
jimitem& jimhash::operator[](const jimstring& key) {

	debug.enter("jimhash", "jimitem& jimhash::operator[](const jimstring&)");

	jimitem &retrn = index(key.use());

	debug.exit("jimhash", "jimitem& jimhash::operator[](const jimstring&)");

	return retrn;
}
const jimitem& jimhash::operator[](const jimstring& key) const {

	debug.enter("jimhash", "const jimitem& jimhash::operator[](const jimstring&) const");

	const jimitem &retrn = index(key.use());

	debug.exit("jimhash", "const jimitem& jimhash::operator[](const jimstring&) const");

	return retrn;
}
jimitem& jimhash::operator[](int theindex) {

	debug.enter("jimhash", "jimitem& jimhash::operator[](int)");

	jimitem &retrn = index(theindex);

	debug.exit("jimhash", "jimitem& jimhash::operator[](int)");

	return retrn;
}

const jimitem& jimhash::operator[](int theindex) const {

	debug.enter("jimhash", "const jimitem& jimhash::operator[](int) const");

	const jimitem &retrn = index(theindex);

	debug.exit("jimhash", "const jimitem& jimhash::operator[](int) const");

	return retrn;
}

jimitem& jimhash::index(const char* key) {

	debug.enter("jimhash", "jimitem& jimhash::index(const char*)");

	debug.print("jimhash", "Retreiving item for key: ", key);

	mayberestructure();

	jimvector<jimvector<jimitem> > &mybuckets = *buckets;

	int bucketindex = computebucketindex(key);

	jimvector<jimitem> &bucket = mybuckets[bucketindex];

	int itemindex = finditemindex(&bucket, key);

	if (itemindex == -1) {

		debug.print("jimhash", "Did not find item");

		bucket += jimitem(key);
		itemindex = bucket.size - 1;
		size++;

		debug.print("jimhash", "Increased size to: ", _(size).use(), " and added ", bucket[itemindex].key.use(), " to position ", _(itemindex).use(), " in bucket ", _(bucketindex).use());
	}
	else {

		debug.print("jimhash", "Found item ");
	}

	debug.exit("jimhash", "jimitem& jimhash::index(const char*)");

	return bucket[itemindex];
}
const jimitem& jimhash::index(const char* key) const {

	debug.enter("jimhash", "const jimitem& jimhash::index(const char*) const");

	debug.print("jimhash", "Retreiving item for key: ", key);

	int itemindex = -1;

	jimvector<jimitem>* bucket;

	if (buckets) {

		int bucketindex = computebucketindex(key);

		bucket = buckets->use(bucketindex);

		itemindex = finditemindex(bucket, key);
	}

	if (itemindex == -1) {

		debug.fail("jimhash", "jimhash::index(const char*) const - Index failed in const context");

		return faildummy;
	}
	else {

		debug.print("jimhash", "Found item ");
	}

	debug.exit("jimhash", "const jimitem& jimhash::index(const char*) const");

	return bucket->index(itemindex);
}
jimitem& jimhash::index(const jimstring& key) {

	debug.enter("jimhash", "jimitem& jimhash::index(const jimstring&)");

	jimitem &retrn = index(key.use());

	debug.exit("jimhash", "jimitem& jimhash::index(const jimstring&)");

	return retrn;
}
const jimitem& jimhash::index(const jimstring& key) const {

	debug.enter("jimhash", "const jimitem& jimhash::index(const jimstring&) const");

	const jimitem &retrn = index(key.use());

	debug.exit("jimhash", "const jimitem& jimhash::index(const jimstring&) const");

	return retrn;
}
jimitem& jimhash::index(int theindex) {

	debug.enter("jimhash", "jimitem& jimhash::index(int)");

	debug.print("jimhash", "Retreiving item for index: ", _(theindex).use());

	mayberestructure();

	if (theindex < 0) {

		theindex = size + theindex;
	}

	if (theindex < size && theindex > -1) {

		int total = 0;

		for (int a = 0; a < buckets->size; a++) {

			jimvector<jimitem>* bucket = buckets->use(a);

			if (theindex < total + bucket->size) {

				debug.exit("jimhash", "jimhash::index(int) const");

				return bucket->index(theindex - total);
			}

			total += bucket->size;
		}
	}

	debug.fail("jimhash", "jimitem& jimhash::index(int)");

	return faildummy;
}
const jimitem& jimhash::index(int theindex) const {

	debug.enter("jimhash", "const jimitem& jimhash::index(int) const");

	debug.print("jimhash", "Retreiving item for index: ", _(theindex).use());

	if (theindex < 0) {

		theindex = size + theindex;
	}

	if (theindex < size && theindex > -1) {

		int total = 0;

		for (int a = 0; a < buckets->size; a++) {

			jimvector<jimitem>* bucket = buckets->use(a);

			if (theindex < total + bucket->size) {

				debug.exit("jimhash", "jimhash::index(int) const");

				return bucket->index(theindex - total);
			}

			total += bucket->size;
		}
	}

	debug.fail("jimhash", "const jimitem& jimhash::index(int) const");

	return faildummy;
}

bool jimhash::exists(const char* key) const {

	debug.enter("jimhash", "bool jimhash::exists(const char*)");

	jimvector<jimitem>* bucket = buckets->use(computebucketindex(key));

	int retrn = finditemindex(bucket, key) != -1;

	debug.exit("jimhash", "bool jimhash::exists(const char*)");

	return retrn;
}

template<class type> type& jimhash::ensure(const char* key, const type& example) {
	
	debug.enter("jimhash", "void jimhash::ensure(const char*, const type*)");
	
	if (!exists(key)) {

		index(key) = type();
	}
	
	type& retrn = index(key);
	
	debug.exit("jimhash", "void jimhash::ensure(const char*, const type&)");
	
	return retrn;
}

bool jimhash::del(const char* key) {

	debug.enter("jimhash", "bool jimhash::del(const char*)");

	mayberestructure();

	jimvector<jimitem>* bucket = &buckets->index(computebucketindex(key));
	int bucketindex = finditemindex(bucket, key);

	if (bucketindex == -1) {

		debug.fail("jimhash", "bool jimhash::del(const char*)");

		return false;
	}

	bucket->del(bucketindex);
	size--;

	debug.exit("jimhash", "bool jimhash::del(const char*)");

	return true;
}

int jimhash::finditemindex(jimvector<jimitem>* bucket, const char* key) const {

	debug.enter("jimhash", "int jimhash::finditemindex(jimvector<jimitem>*, const char*)");

	for (int a = 0; a < bucket->size; a++) {

		jimstring &testkey = bucket->index(a).key;

		debug.print("jimhash", "Trying: ", testkey.use(), " to match value: ", key);

		if (testkey == key) {

			debug.print("jimhash", "Mastched");
			debug.exit("jimhash", "int jimhash::finditemindex(jimvector<jimitem>*, const char*)");

			return a;
		}
		else {

			debug.print("jimhash", "Did not Match");
		}
	}

	debug.print("jimhash", key, " was not found");

	debug.exit("jimhash", "int jimhash::finditemindex(jimvector<jimitem>*, const char*)");

	return -1;
}

void jimhash::mayberestructure() {

	debug.enter("jimhash", "void jimhash::mayberestructure()");

	if (
		buckets
		&& size * 2 < buckets->size
		&& size > (buckets->size - 10) / 4
	) {

		debug.print("jimhash", "No need to restructure");

		debug.exit("jimhash", "void jimhash::mayberestructure()");

		return;
	}

	int newbucketcount = size > 4 ? size * 4 : 10;


	debug.print("jimhash", "Resizing bucket count from ", _(buckets ? buckets->size : 0).use(), " to ", _(newbucketcount).use(), " to support ", _(size).use(), " items");


	jimvector<jimvector<jimitem> > *newbuckets = new jimvector<jimvector<jimitem> >(newbucketcount);
	jimvector<jimvector<jimitem> > &newbucketlist = *newbuckets;

	if (buckets) {

		jimvector<jimvector<jimitem> > &bucketlist = *buckets;

		for (int oldbucketindex = 0; oldbucketindex < bucketlist.size; oldbucketindex++) {

			//ToDo: make this more efficient.
			jimvector<jimitem> &bucket = bucketlist[oldbucketindex];

			for (int olditemindex = 0; olditemindex < bucket.size; olditemindex++) {

				jimitem& olditem = bucket[olditemindex];

				int newbucketindex = computebucketindex(olditem.key.use(), newbucketcount);

				newbucketlist[newbucketindex] += olditem;
			}
		}

		delete buckets;
	}

	buckets = newbuckets;

	debug.exit("jimhash", "void jimhash::mayberestructure()");
}

int jimhash::computebucketindex(const char* key, int bucketsize) const {

	debug.enter("jimhash", "int jimhash::computebucketindex(const char*, int)");

	if (bucketsize < 0) {

		bucketsize = buckets->size;
	}

	int sum = 0;
	unsigned char* thekey = (unsigned char*)key;

	for (int a = 0; thekey[a]; a++) {

		sum += thekey[a];
	}

	int retrn = sum % bucketsize;

	debug.exit("jimhash", "int jimhash::computebucketindex(const char*, int)");

	return retrn;
}

jimvector<jimstring> jimhash::getkeys() const {

	debug.print("jimhash", "jimvector<jimstring> jimhash::getkeys() const");

	jimvector<jimstring> retrn;

	for (int bucketindex = 0; bucketindex < buckets->size; bucketindex++) {

		jimvector<jimitem> &bucket = buckets->index(bucketindex);

		for (int itemindex = 0; itemindex < bucket.size; itemindex++) {

			retrn += bucket[itemindex].key;
		}
	}

	debug.print("jimhash", "jimvector<jimstring> jimhash::getkeys() const");

	return retrn;
}

template<class type>
bool jimhash::params(type& variableref, const char* paramstring) const {

	debug.enter("web", "SimpleWeb::params(const jimhash&, type&, const char*)");
	const char* purestring = &paramstring[1];

	if (exists(purestring)) {

		variableref = (type)index(purestring);

		debug.print("web", "Found param: ", purestring);
	}
	else if (paramstring[0] == '+') {

		debug.print("web", "Could not find param: ", purestring);
		debug.fail("web", "SimpleWeb::params(const jimhash&, type&, const char*)");

		return false;
	}
	else {

		debug.print("web", "Optional param not found: ", purestring);
	}

	debug.exit("web", "SimpleWeb::params(const jimhash&, type&, const char*)");

	return true;
}

template<class type, class... args>
bool jimhash::params(type& variableref, const char* paramstring, args&... myargs) const {

	return (
		params(variableref, paramstring)
		&& params(myargs...)
	);
}

template <class streamtype, typename datatype>
void jimhash::save(streamtype& stream, const datatype& example) const {

	jimvector<jimstring> thekeys(getkeys());

	stream << thekeys.size;

	for (int a = 0; a < thekeys.size; a++) {

		jimstring &thekey = thekeys[a];

		datatype &theuser = index(thekey);

		stream << thekey << theuser;
	}
}

template <class streamtype, typename datatype>
void jimhash::load(streamtype& stream, const datatype& example) {

	int keycount;

	stream >> keycount;

	for (int a = 0; a < keycount; a++) {

		jimstring thekey;
		datatype thevalue;

		stream >> thekey >> thevalue;

		index(thekey) = thevalue;
	}
}
