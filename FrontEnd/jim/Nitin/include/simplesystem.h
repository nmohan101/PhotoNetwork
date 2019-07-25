
class SimpleSystem {
public:

	jimstring runcommand(const char* thecommand);

	__int64 gettotalram();
	__int64 getfreeram();
	__int64 getusedram();
};

SimpleSystem simplesystem;


////////////////////////////////
//  Can finally turn all these setup lines into something like
//  jimstring username, newmesasge, lastreceivedmessageidstring
//  if (!params(fdat,
//  	username, "username", "u 20",
//  	newmessage, "newmessage", "s 10000",
//  	lastreceivedmesageidstring, "lastreceivedmessageid", "#"
//  )) {
//
//  	return errorrequest();
//  }

/*
Types:
	s: string [minlength] maxlength
	#: string-integer [minvalue] maxvalue
	u: string-username [minlength] maxlength
	i; int32 [minvalue] maxvalue
	f; float32 [minvalue] maxvalue
*/




/*////////////////////////////////////////////////

bool validatedata(jimstring& thestring, jimitem& theitem, const char* datatypeparam, bool setvalue = false) {

	debug.enter("system", "SimpleSystem::validatedata");

	jimstring datatype = datatypeparam;

	jimstring &stringref = theitem;
// Make function to automatically cast to ref!!! :)
// ToDo: Make sure all our objects can be reflexively assigned without dying.
// If it doesn't workm just cast with templatetype here

	// Check datatype validity

	jimvector<jimstring> datatypeparams = datatype.split(" ");
	jimstring &stringcode = datatypeparams[0];
	int minsize = datatypeparams.size > 2 ? datatypeparams[1] : -1;
	int maxsize = datatypeparams.size > 1 ? datatypeparams[-1] : -1;

	if (stringcode == "s") {

		// Any string. Just check length
	}
	else if (stringcode == "u") {

		// Username
	}
	else if (stringcode == "#") {

		// String that is an int

		if (!stringref.isint()) {

			return false;
		}
	}
	else {

		// throw!
	}

	if (
		(
			minsize == -1
			|| stringref.len >= minsize
		)
		&& (
			maxsize == -1
			|| stringref.len <= maxsize
		)
	) {

		// Invalid Size

		return false;
	}

	if (setvalue) {

		thestring = stringref;
	}

	debug.exit("system", "SimpleSystem::validatedata");

	return true;
}


template<class templatetype, class... args>
void params(jimhash& paramhash, templatetype& localvar, const char* paramkey, const char* datatype, args... myargs) {

	debug.enter("simplesystem", "params(jimhash&, templatetype&, const char*, const char*, args...)");

	// verify that the type is a valid type, and that it meets the data requirements
	// These should be the same definitions as database variables

	validatedata(localvar, paramhash[paramkey], datatype, true);

	params(paramhash, myargs...);

	debug.exit("simplesystem", "params(jimhash&, templatetype&, const char*, const char*, args...)");
}

//*/