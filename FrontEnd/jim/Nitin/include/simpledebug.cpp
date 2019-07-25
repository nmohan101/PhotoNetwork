simpledebug::simpledebug() {

	tabcount = 0;
	debugging = false;
}

template<class... args>
void simpledebug::msgrecurse(const char* text) {

	if (text) {

		msg(text);
	}
}
template<class... args>
void simpledebug::msgrecurse(const char* text, args... myargs) {

	msgrecurse(text);
	msgrecurse(myargs...);
}

template<class... args>
void simpledebug::debugmsg(args... myargs) {

	if (tabcount > 0) {

		jimstring msgtext(tabcount, (char*)"\t");
		msg(msgtext.use());
	}

	msgrecurse(myargs...);
	msg("\n");
}

template<class... args>
void simpledebug::enter(const char* type, args... myargs) {

	if (debugging) {

		debugging = false;

		if (types.find(" " + _(type) + " ") > -1) {

			debugmsg(myargs..., (char*)" {");
			tabcount++;
		}

		debugging = true;
	}
}

template<class... args>
void simpledebug::exit(const char* type, args... myargs) {

	if (debugging) {

		debugging = false;

		if (types.find(" " + _(type) + " ") > -1) {

			tabcount--;
			debugmsg((char*)"} ", myargs...);
		}

		debugging = true;
	}
}

template<class... args>
void simpledebug::fail(const char* type, args... myargs) {

	if (debugging) {

		debugging = false;

		if (types.find(" " + _(type) + " ") > -1) {

			tabcount--;
			debugmsg((char*)"}*FAILED* ", myargs...);
		}

		debugging = true;
	}
}

template<class... args>
void simpledebug::print(const char* type, args... myargs) {

	if (debugging) {

		debugging = false;

		if (types.find(" " + _(type) + " ") > -1) {

			debugmsg((char*)"- ", myargs...);
		}

		debugging = true;
	}
}