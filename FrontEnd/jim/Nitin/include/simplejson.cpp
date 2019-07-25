
bool SimpleJSON::isnumberchar(char thechar) {

	return (
		(
			thechar >= '0'
			&& thechar <= '9'
		)
		|| thechar == '+'
		|| thechar == '-'
		|| thechar == '.'
		|| thechar == 'e'
		|| thechar == 'E'
	);
}

/***************************************************************************************************
* specialchars
*	ToDo: Change specialchars to be way more efficient. Use the first loop to find the nfirst non-whitespace char
*
*	Moves along the "text" jimstring to return the first non-whitespace, json character, then returns
*		the index of the "positions" jimvector in which that character was returned. Why so convoluded?
****************************************************************************************************/
int SimpleJSON::specialchars(jimstring& text, int& position, jimvector<int>& positions) {

	// Find the position of the various types of json special characters

	positions[0] = text.find("\"", position);	// "
	positions[1] = text.find("\\", position);	// backslash
	positions[2] = text.find(":", position);	// :
	positions[3] = text.find("[", position);	// [
	positions[4] = text.find("]", position);	// ]
	positions[5] = text.find("{", position);	// {
	positions[6] = text.find("}", position);	// }
	positions[7] = text.find(",", position);	// ,
	positions[8] = -1;								// #+-.eE

	// Find the fposition of the first number-related character, starting from text.position
	for (int a = position; a < text.len; a++) {

		if (isnumberchar(text[a])) {

			positions[8] = a;
			break;
		}
	}

	int smallestindex = -1;
	int smallest = -1;

	// Find the position and type of the closest character after text.position, and possibly after whitespace.
	for (int a = 0; a < positions.size; a++) {

		if (
			positions[a] > -1 // This character type was foud before the end of the string
			&& (
				smallest == -1 // This is the first loop
				|| positions[a] < smallest // This position is closer than the last closest
			)
		) {

			smallest = positions[a];
			smallestindex = a;
		}
	}

	// Set the current position of the text variable to reflect the charater type that is to be returned
	if (smallestindex == -1) {

		debug.print("json", "specialchars: found end of string");

		position = text.len;
	}
	else {

		position = positions[smallestindex] + 1;

		debug.print("json", "specialchars: found ", text.seltext(position - 1, 1).use());
	}

	return smallestindex;
}

jimvector<jimitem> SimpleJSON::getlist(jimstring& text, int& position, jimvector<int>& positions) {

	debug.enter("json", "getlist");


	jimvector<jimitem> retrn;
	int nextcharindex;

	while (1) {

		jimitem value;

		if (getvalue(text, position, positions, value).failed) {

			break;
		}

		debug.print("json", "Got value");

		retrn += value.give();

		nextcharindex = specialchars(text, position, positions);

		if (nextcharindex == 4) {		// ]

			debug.exit("json", "getlist found ]");

			return retrn;
		}
		else if (nextcharindex != 7) {	// ,

			break;
		}

		debug.print("json", "Expecting another value");
	}

	debug.fail("json", "getlist");

	retrn.fail();

	return retrn;
}

jimhash SimpleJSON::getobject(jimstring& text, int& position, jimvector<int>& positions) {


	debug.enter("json", "getobject");

	jimhash retrn;
	int nextcharindex;

	while (1) {

		nextcharindex = specialchars(text, position, positions);

		if (nextcharindex != 0) {		// "

			break;
		}

		jimstring key = getstring(text, position, positions);

		if (key.failed) {

			break;
		}

		nextcharindex = specialchars(text, position, positions);

		if (nextcharindex != 2) {	// :

			break;
		}

		if (getvalue(text, position, positions, retrn[key.give()]).failed) {

			break;
		}

		nextcharindex = specialchars(text, position, positions);

		if (nextcharindex == 6) {		// }

			debug.exit("json", "getobject");

			return retrn;
		}
		else if (nextcharindex != 7) {	// ,

			break;
		}
	}

	debug.fail("json", "getobject");

	retrn.fail();

	return retrn;
}

jimitem& SimpleJSON::getvalue(jimstring& text, int& position, jimvector<int>& positions, jimitem& retrn) {

	debug.enter("json", "getvalue");

	int nextcharindex;

	nextcharindex = specialchars(text, position, positions);

	if (nextcharindex == 5)	{	// {

		retrn = getobject(text, position, positions);
	}
	else if (nextcharindex == 3) {	// [

		retrn = getlist(text, position, positions);
	}
	else if (nextcharindex == 0) {	// "

		jimstring valuestring = getstring(text, position, positions);

		retrn = valuestring.give();
	}
	else if (nextcharindex == 8) {	// #+-.eE  (Numbers)

		retrn = getnumber(text, position, positions);
	}
	else {

		debug.fail("json", "getvalue");

		retrn.fail();

		return retrn;
	}

	debug.exit("json", "getvalue");

	return retrn;
}

jimstring SimpleJSON::getstring(jimstring& text, int& position, jimvector<int>& positions) {

	debug.enter("json", "getstring");

	jimstring retrn;
	int startposition = position;
	bool endquotefound = false;

	do {
		position = text.find("\"", position) + 1;

		int slashposition = position - 2;

		while(text[slashposition--] == '\\') {}

		endquotefound = (position - slashposition) % 2;
	} while (
		position
		&& !endquotefound
	);

	if (position) {

			retrn = text.seltext(startposition, position - startposition - 1);
	}
	else {

			debug.fail("json", "getstring");

			retrn.fail();

			return retrn;
	}

	debug.exit("json", "getstring");

	return retrn;
}

float SimpleJSON::getnumber(jimstring& text, int& position, jimvector<int>& positions) {

	debug.enter("json", "getnumber");

	int startposition = position - 1;

	do {

		if (!isnumberchar(text[position])) {

			break;
		}
	} while (++position < text.len);

	jimstring retrnstring = text.seltext(startposition, position - startposition);
//msg(retrn.use());

	float retrn = (float)atof(retrnstring.use());

	debug.exit("json", "getnumber");

	return retrn;
}

jimitem SimpleJSON::parse(const char* json) {

	jimitem retrn;

	jimstring text;
	jimvector<int> positions(9);

	text.borrow(json);
	int position = 0;

	int nextchar = specialchars(text, position, positions);

	if (nextchar == 5) {	// {

		retrn = getobject(text, position, positions);
	}
	else if (nextchar == 3) {	// [

		retrn = getlist(text, position, positions);
	}
	else {

		retrn.fail();
	}

	return retrn;
}

jimstring SimpleJSON::stringify(const jimitem& jsonitem, int prettytabcount) {

	jimstring retrn;

	jimhash temphash;
	jimvector<jimitem> tempvector;
	jimstring tempstring;
	int tempint;
	float tempfloat;
	
	if (jsonitem.type == jimitem::gettypestring(temphash)) {

		retrn = stringify((jimhash&)jsonitem);
	}
	else if (jsonitem.type == jimitem::gettypestring(tempvector)) {

		retrn = stringify((jimvector<jimitem>)jsonitem);
	}
	else if (jsonitem.type == jimitem::gettypestring(tempstring)) {

		retrn = "\"" + (jimstring)jsonitem + "\"";
	}
	else if (jsonitem.type == jimitem::gettypestring(tempint)) {

		retrn = _((int)jsonitem);
	}
	else if (jsonitem.type == jimitem::gettypestring(tempfloat)) {

		retrn = _((float)jsonitem);
	}
	else {

		msgln("SimpleCoding Runtime Error: Tried to json-stringify an object of type \"" + jsonitem.type + "\" So Add it to the list in SimpleJSON::Stringify(const jimitem&, int)");

		retrn.fail();
	}

	return retrn;
}

jimstring SimpleJSON::stringify(const jimhash& jsonhash, bool pretty) {

	jimstring retrn;

	jimvector<jimstring> keys = jsonhash.getkeys();

	for (int a = 0; a < keys.size; a++) {

		jimstring &key = keys[a];

		const jimitem &theitem = jsonhash[key];

		if (a) {

			retrn += ",";
		}

		retrn += "\"" + key + "\":" + stringify(theitem);
	}

	retrn = "{" + retrn + "}";

	return retrn;
}

jimstring SimpleJSON::stringify(const jimvector<jimitem>& jsonlist, bool pretty) {

	jimstring retrn;

	for (int a = 0; a < jsonlist.size; a++) {

		jimitem &theitem = jsonlist[a];

		if (a) {

			retrn += ",";
		}

		retrn += stringify(theitem);
	}

	retrn = "[" + retrn + "]";

	return retrn;
}
