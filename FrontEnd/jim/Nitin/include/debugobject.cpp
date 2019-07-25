
debugobject::debugobject() {

}

debugobject::debugobject(char* text) {

	value = text;
}

debugobject::debugobject(const debugobject& match) {

	value = match.value;

	if (value.len) {

		debug.print("debugobject", "Created and Copied DebugObject -> ", value.use());
	}
	else {

		debug.print("debugobject", "Created and Copied Empty DebugObject");
	}
}

debugobject::debugobject(debugobject&& match) {

	value.steal(match.value);

	if (value.len) {

		debug.print("debugobject", "Created and Stole DebugObject -> ", value.use());
	}
	else {

		debug.print("debugobject", "Created and Stole Empty DebugObject");
	}
}

debugobject::~debugobject() {

	if (value.len) {

		debug.print("debugobject", "Destroyed DebugObject -> ", value.use());
	}
	else {

		debug.print("debugobject", "Destroyed Empty DebugObject");
	}
}

debugobject& debugobject::operator=(const debugobject& match) {

	value = match.value;

	if (value.len) {

		debug.print("debugobject", "Copied DebugObject -> ", value.use());
	}
	else {

		debug.print("debugobject", "Copied Empty DebugObject");
	}

	return *this;
}

debugobject& debugobject::operator=(debugobject&& match) {

	value.steal(match.value);

	if (value.len) {

		debug.print("debugobject", "Stole DebugObject -> ", value.use());
	}
	else {

		debug.print("debugobject", "Stole Empty DebugObject");
	}

	return *this;
}