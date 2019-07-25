
void jimitem::init() {

	debug.enter("jimitem", "void jimitem::init()");

	type.destroy();
	value = 0;
	borrowed = false;
	failed = false;

//////////////////////////////
	destructorcallback = 0;
	copycallback = 0;
//////////////////////////////

	debug.exit("jimitem", "void jimitem::init()");
}

jimitem::jimitem() {

	debug.enter("jimitem", "jimitem::jimitem()");

	init();

	debug.exit("jimitem", "jimitem::jimitem()");
}

template <class templatetype>
jimitem::jimitem(const char* newkey, const templatetype& valueref) {

	debug.enter("jimitem", "jimitem::jimitem(const char*, const templatetype&)");

	init();

	key = newkey;
	value = (void*)new templatetype(valueref);
	settypestring(valueref);
//////////////////////////
	setcallbacks(&valueref);
////////////////////////////
	debug.print("jimitem", "created jimitem with key=", key.use(), " and type=", type.use());

	debug.exit("jimitem", "jimitem::jimitem(const char*, const templatetype&)");
}

jimitem::jimitem(const jimitem& match) {

	debug.enter("jimitem", "jimitem::jimitem(const jimitem&)");

	init();

	copy(match);

	debug.print("jimitem", "created jimitem with key=", key.use(), " and type=", type.use());

	debug.exit("jimitem", "jimitem::jimitem(const jimitem&)");
}
jimitem::jimitem(jimitem&& match) {

	debug.enter("jimitem", "jimitem::jimitem(jimitem&&)");

	init();

	steal(match);

	debug.print("jimitem", "created jimitem with key=", key.use(), " and type=", type.use());

	debug.exit("jimitem", "jimitem::jimitem(jimitem&&)");
}
jimitem::jimitem(const char* newkey) {

	debug.enter("jimitem", "jimitem::jimitem(const char*)");

	init();

	key = newkey;

	debug.print("jimitem", "created jimitem with key=", key.use());

	debug.exit("jimitem", "jimitem::jimitem(const char*)");
}
jimitem::jimitem(const jimstring& newkey) {

	debug.enter("jimitem", "jimitem::jimitem(const jimstring&)");

	init();

	key = newkey;

	debug.print("jimitem", "created jimitem with key=", key.use());

	debug.exit("jimitem", "jimitem::jimitem(const jimstring&)");
}
jimitem::jimitem(jimstring&& newkey) {

	debug.enter("jimitem", "jimitem::jimitem(jimstring&&)");

	init();

	key = std::move(newkey);

	debug.print("jimitem", "created jimitem with key=", key.use());

	debug.exit("jimitem", "jimitem::jimitem(jimstring&&)");
}

///////////////////////////////////////
template <class templatetype>
void jimitem::setcallbacks(const templatetype* examplepointer) {

	debug.print("jimitem", "void jimitem::setcallbacks(const templatetype*)");

	destructorcallback = [](void* destructval) {

		debug.enter("jimitem", "jimitem::[destructorcallback](void*)");

		delete (templatetype*)destructval;

		debug.exit("jimitem", "jimitem::[destructorcallback](void*)");
	};

	copycallback = [](void*& destination, const void* source) {

		debug.enter("jimitem", "jimitem::[copycallback](void*&, const void*)");

		destination = new templatetype();

		*(templatetype*)destination = *(templatetype*)source;

		debug.exit("jimitem", "jimitem::[copycallback](void*&, void*)");
	};
}
///////////////////////////////////////////////////////////

jimitem::~jimitem() {

	debug.enter("jimitem", "jimitem::~jimitem()");

	destroy();

	debug.exit("jimitem", "jimitem::~jimitem()");
}

jimitem& jimitem::destroy() {

	debug.enter("jimitem", "jimitem::destroy()");

	if (value && !borrowed && destructorcallback) {

		debug.print("jimitem", "invoking destructor for ", type.use());

		destructorcallback(value);
	}

	init();

	debug.exit("jimitem", "jimitem::destroy()");

	return *this;
}
/*
jimitem& jimitem::destroy() {

	if (value && !borrowed) {

		// ToDo: this could be avoided by switching to malloc and free and saving the objects destructor function pointer with a lamda function:
		// Should be able to use the same technique for copy
		// Don't need to do it for move because move is just a ptr copy


			// Called indirectly from deferred_heap::make<T>.
			// Here, t is a T&.
			//dtors.push_back({
			//	std::addressof(t), // address of object
			//	[](const void* x) { static_cast<const T*>(x)->~T(); }
			//});                    // dtor to invoke


		int exampleint;
		float examplefloat;
		jimstring examplejimstring;
		jimvector<int> examplejimvectorint;
		jimitem examplejimitem;
		jimvector<jimitem> examplejimvectorjimitem;
		jimvector<jimstring> examplejimvectorjimstring;
		jimhash examplejimhash;
		time_t exampletime_t;

		if (type == gettypestring(exampleint)) {

			delete (int*)value;
		}
		else if (type == gettypestring(examplefloat)) {

			delete (float*)value;
		}
		else if (type == gettypestring(examplejimstring)) {

			delete (jimstring*)value;
		}
		else if (type == gettypestring(examplejimvectorint)) {

			delete (jimvector<int>*)value;
		}
		else if (type == gettypestring(examplejimitem)) {

			delete (jimitem*)value;
		}
		else if (type == gettypestring(examplejimvectorjimitem)) {

			delete (jimvector<jimitem>*)value;
		}
		else if (type == gettypestring(examplejimvectorjimstring)) {

			delete (jimvector<jimstring>*)value;
		}
		else if (type == gettypestring(examplejimhash)) {

			delete (jimhash*)value;
		}
		else if (type == gettypestring(exampletime_t)) {

			delete (time_t*)value;
		}
		else {

			msg("SimpleCoding runtime error: Attempted to autodelete a jimitem with type ("
				+ type + "). This could cause a memory leak. Modify jimitem::destroy().\n");
		}

		debug.print("jimitem", "jimitem::destroy()");
	}

	init();

	return *this;
}

//*/

jimitem jimitem::give() {

	debug.enter("jimitem", "jimitem jimitem::give()");

	jimitem retrn(std::move(*this));

	debug.exit("jimitem", "jimitem jimitem::give()");

	return retrn;
}

const jimitem& jimitem::fail() {

	debug.enter("jimitem", "jimitem jimitem::fail()");

	destroy();
	failed = true;

	debug.exit("jimitem", "jimitem jimitem::fail()");

	return *this;
}

jimitem& jimitem::copy(const jimitem& match) {

	debug.enter("jimitem", "jimitem& jimitem::copy(const jimitem&)");

	destroy();

	type = match.type;
	key = match.key;

	if (match.value && match.copycallback) {

		debug.print("jimitem", "invoking deep copy of type ", type.use());

		match.copycallback(value, match.value);
	}

	borrowed = false;
	failed = match.failed;
	destructorcallback = match.destructorcallback;
	copycallback = match.copycallback;

	debug.exit("jimitem", "jimitem& jimitem::copy(const jimitem&)");
}
/*
jimitem& jimitem::copy(const jimitem& match) {

	debug.print("jimitem", "jimitem& jimitem::copy(const jimitem&)");

	type = match.type;
	key = match.key;

	if (match.value) {

		int exampleint;
		float examplefloat;
		jimstring examplejimstring;
		jimhash examplejimhash;
		jimvector<jimitem> examplejimvectorjimitem;
		jimvector<jimstring> examplejimvectorjimstring;
		time_t exampletime_t;

		if (type == gettypestring(exampleint)) {

			value = (void*)new int(*(int*)match.value);
		}
		else if (type == gettypestring(examplejimstring)) {

			value = (void*)new jimstring(*(jimstring*)match.value);
		}
		else if (type == gettypestring(examplefloat)) {

			value = (void*)new float(*(float*)match.value);
		}
		else if (type == gettypestring(examplejimhash)) {

			value = (void*)new jimhash(*(jimhash*)match.value);
		}
		else if (type == gettypestring(examplejimvectorjimitem)) {

			value = (void*)new jimvector<jimitem>(*(jimvector<jimitem>*)match.value);
		}
		else if (type == gettypestring(examplejimvectorjimstring)) {

			value = (void*)new jimvector<jimstring>(*(jimvector<jimstring>*)match.value);
		}
		else if (type == gettypestring(exampletime_t)) {

			value = (void*)new time_t(*(time_t*)match.value);
		}
		else {

			msg("SimpleCoding runtime error: Attempted to autocopy a jimitem with type ("
				+ type + "). This WILL cause segmentation faults. Modify jimitem::copy(const jimitem&).\n");
		}
	}

	borrowed = false;
	failed = match.failed;

	return *this;
}
//*/
jimitem& jimitem::steal(jimitem& match) {

	debug.enter("jimitem", "jimitem& jimitem::steal(jimitem&)");

	destroy();

	if (match.borrowed) {

		debug.print("jimitem", "Can't steal from borrowed. Copying instead.");

		copy(match);
	}
	else {

		type.steal(match.type);
		key.steal(match.key);
		value = match.value;
		failed = match.failed;
		destructorcallback = match.destructorcallback;
		copycallback = match.copycallback;
	}

	match.init();

	debug.exit("jimitem", "jimitem& jimitem::steal(jimitem&)");

	return *this;
}
jimitem& jimitem::steal(jimitem&& match) {

	debug.enter("jimitem", "jimitem& jimitem::steal(jimitem&&)");

	jimitem& retrn = steal(match);

	debug.exit("jimitem", "jimitem& jimitem::steal(jimitem&&)");

	return retrn;
}

jimitem& jimitem::operator=(const jimitem& match) {

	debug.enter("jimitem", "jimitem& jimitem::operator=(const jimitem&)");

	copy(match);

	debug.exit("jimitem", "jimitem& jimitem::operator=(const jimitem&)");

	return *this;
}
jimitem& jimitem::operator=(jimitem&& match) {

	debug.enter("jimitem", "jimitem& jimitem::operator=(jimitem&&)");

	steal(match);

	debug.exit("jimitem", "jimitem& jimitem::operator=(jimitem&&)");

	return *this;
}
template <class templatetype>
jimitem& jimitem::operator=(const templatetype& valueref) {

	debug.enter("jimitem", "jimitem& jimitem::operator=(const templatetype&)");


	destroy();
	value = (void*)new templatetype(valueref);

	settypestring(valueref);

	setcallbacks(&valueref);

	debug.print("jimitem", "Set value for key: ", key.use(), " as type: ", type.use());

	debug.exit("jimitem", "jimitem& jimitem::operator=(const templatetype&)");

	return *this;
}

template <class templatetype>
jimstring jimitem::gettypestring(const templatetype& thetype) {

	jimstring retrn((char*)typeid(templatetype).name());

	debug.print("jimitem", "jimitem::gettypestring() ", retrn.use());

	return retrn;//.replace(" const ", " ");
}
template <class templatetype>
jimitem& jimitem::settypestring(const templatetype& thetype) {

	type = gettypestring(thetype);

	debug.print("jimitem", "jimitem::settypestring() ", type.use());

	return *this;
}

/*template <class templatetype>
jimitem::operator templatetype() const {

	debug.print("jimitem", "jimitem::operator(typecast)");

	templatetype typeinstance;
	jimstring typestring(gettypestring(typeinstance));

	if (typestring == type) {

		return *(templatetype*)value;
	}

	msg("SimpleCoding runtime error: Hash value of type: (" + type
		+ ") cannot be cast to type (" + typestring + ")"
	);

	return typeinstance;
}*/
template <class templatetype>
jimitem::operator templatetype&() const {

	debug.enter("jimitem", "jimitem::operator typecast()");

	templatetype typeinstance;
	jimstring typestring(gettypestring(typeinstance));

	if (typestring != type) {

		msg("SimpleCoding runtime error: Hash value of type: (" + type
			+ ") cannot be cast to type (" + typestring + ")"
		);
	}

	debug.exit("jimitem", "jimitem::operator typecast()");

	return *(templatetype*)value;
}

jimitem& jimitem::operator[](int theindex) {

	debug.enter("jimitem", "jimitem& jimitem::operator[](int)");

	jimitem &retrn = index(theindex);

	debug.exit("jimitem", "jimitem& jimitem::operator[](int)");

	return retrn;
}
const jimitem& jimitem::operator[](int theindex) const {

	debug.enter("jimitem", "const jimitem& jimitem::operator[](int) const");

	const jimitem &retrn = index(theindex);

	debug.exit("jimitem", "const jimitem& jimitem::operator[](int) const");

	return retrn;
}
jimitem& jimitem::operator[](const char* thekey) {

	debug.enter("jimitem", "jimitem& jimitem::operator[](const char*)");

	jimitem &retrn = index(thekey);

	debug.exit("jimitem", "jimitem& jimitem::operator[](const char*)");

	return retrn;
}
const jimitem& jimitem::operator[](const char* thekey) const {

	debug.enter("jimitem", "const jimitem& jimitem::operator[](const char*) const");

	const jimitem &retrn = index(thekey);

	debug.exit("jimitem", "const jimitem& jimitem::operator[](const char*) const");

	return retrn;
}
jimitem& jimitem::operator[](const jimstring& thekey) {

	debug.enter("jimitem", "jimitem& jimitem::operator[](const jimstring&)");

	jimitem &retrn = index(thekey.use());

	debug.exit("jimitem", "jimitem& jimitem::operator[](const jimstring&)");

	return retrn;
}
const jimitem& jimitem::operator[](const jimstring& thekey) const {

	debug.enter("jimitem", "const jimitem& jimitem::operator[](const jimstring&) const");

	const jimitem &retrn = index(thekey.use());

	debug.exit("jimitem", "const jimitem& jimitem::operator[](const jimstring&) const");

	return retrn;
}

jimitem& jimitem::index(int theindex) {

	debug.enter("jimitem", "jimitem::operator[int]");

	jimvector<jimitem> examplejimvectorjimitem;
	jimhash examplejimhash;

	if (type == gettypestring(examplejimvectorjimitem)) {

		debug.exit("jimitem", "} Was jimvector of jimitem");

		return ((jimvector<jimitem>*)value)->index(theindex);
	}
	else if (type == gettypestring(examplejimhash)) {

		debug.exit("jimitem", "} Was jimhash");

		return ((jimhash*)value)->index(theindex);
	}

	debug.fail("jimitem", "jimitem type was neither jimvector<item> nor jimhash");

	return *this;
}
const jimitem& jimitem::index(int theindex) const {

	debug.enter("jimitem", "jimitem::operator[int] const");

	jimvector<jimitem> examplejimvectorjimitem;
	jimhash examplejimhash;

	if (type == gettypestring(examplejimvectorjimitem)) {

		debug.exit("jimitem", "} Was jimvector of jimitem");

		return ((jimvector<jimitem>*)value)->index(theindex);
	}
	else if (type == gettypestring(examplejimhash)) {

		debug.exit("jimitem", "} Was jimhash");

		return ((jimhash*)value)->index(theindex);
	}

	debug.fail("jimitem", "jimitem type was neither jimvector<item> nor jimhash");

	return *this;
}
jimitem& jimitem::index(const char* thekey) {

	debug.enter("jimitem", "jimitem::operator[const char*]");

	jimhash examplejimhash;

	if (type == gettypestring(examplejimhash)) {

		debug.exit("jimitem", "} Was type ", type.use());

		return ((jimhash*)value)->index(thekey);
	}

	debug.fail("jimitem", "jimitem type was not jimhash");

	return *this;
}
const jimitem& jimitem::index(const char* thekey) const {

	debug.enter("jimitem", "jimitem::operator[const char*] const");

	jimhash examplejimhash;

	if (type == gettypestring(examplejimhash)) {

		debug.exit("jimitem", "} Was type ", type.use());

		return ((jimhash*)value)->index(thekey);
	}

	debug.fail("jimitem", "jimitem type was not jimhash");

	return *this;;
}
jimitem& jimitem::index(const jimstring& thekey) {

	debug.enter("jimitem", "jimitem& jimitem::index(const jimstring&)");

	jimitem &retrn = index(thekey.use());

	debug.exit("jimitem", "jimitem& jimitem::index(const jimstring&)");

	return retrn;
}
const jimitem& jimitem::index(const jimstring& thekey) const {

	debug.enter("jimitem", "const jimitem& jimitem::index(const jimstring&) const");

	const jimitem &retrn = index(thekey.use());

	debug.exit("jimitem", "const jimitem& jimitem::index(const jimstring&) const");

	return retrn;
}

template <class templatetype>
templatetype& operator<(templatetype& destination, const jimitem& source) {

	debug.enter("jimitem", "operator=(templatetype&, const jimitem&)");

	if (!gettypestring(destination) == source.type) {

		msgln("Type mismatch in templatetype& operator=(templatetype& destination, const jimitem& source) '" + gettypestring(destination) + "'='" + source.type + "'");
	}


	destination = *(templatetype*)source.value;

	debug.exit("jimitem", "operator=(templatetype&, const jimitem&)");

	return destination;
}

template <class templatetype>
templatetype& operator<(templatetype& destination, jimitem&& source) {

	debug.enter("jimitem", "operator=(templatetype&, jimitem&&)");

	if (!gettypestring(destination) == source.type) {

		debug.print("jimitem", "Type mismatch in templatetype& operator=(templatetype& destination, const jimitem& source) '" + gettypestring(destination) + "'='" + source.type + "'");
	}

	destination = std::move(*(templatetype*)source.value);

	debug.exit("jimitem", "operator=(templatetype&, jimitem&&)");

	return destination;
}
