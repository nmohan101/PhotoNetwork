
simpleuser::simpleuser() {

}
simpleuser::simpleuser(const char* username, const char* thepassword) {

	jimstring password;
	password.borrow(thepassword);

	this->username = username;
	this->passwordmd5 = password.md5();
}

bool simpleuser::login(const jimstring& password, const jimstring& accesskey, jimstring& accesskeyreturn, jimstring& errorreturn) {

	//ToDo revisit possibility to have multiple accesskeys for multiple login stations.
	//	This version should only allow last stay logged in attempt to stay logged in
	//	... but that isn't even coded yet. now each login will invalidate all old stayloggedin cookies
	// ToDo: Different error messages for when password isn't given and access key is wrong and stuff.
	if (
		accesskey.len
		&& accesskey == this->accesskey
	) {

		accesskeyreturn = accesskey;

		return true;
	}

	if (password.md5() == passwordmd5) {

		this->accesskey = "randomstring";
		accesskeyreturn = this->accesskey;

msgln("ToDO: in simpleuser::login accesskey is not real. should be 16bytes");
		return true;
	}

	errorreturn = "Password was not correct";

	return false;
}

//Stream Overload
template <class type>
type& operator>>(type& stream, simpleuser& info) {

	stream >> info.username >> info.passwordmd5 >> info.accesskey;

	return stream;
}

template <class type>
type& operator<<(type& stream, const simpleuser& info) {

	stream << info.username << info.passwordmd5 << info.accesskey;

	return stream;
}
