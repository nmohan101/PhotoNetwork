
void msg(const char* message);
void msgln(const char* message);
void msg(int num);


#ifdef _WIN32

	void msg(const char* message) {

		MessageBox(0, message, "Message", MB_TASKMODAL | MB_OK);
	}

#elif __linux__

	void msg(const char* message) {

		printf("%s", message);
	}

	void msgln(const char* message) {

		msg(message);
		msg("\n");
	}

#endif

void msg(int num) {

	// Works in Win32, but not ubuntu:
		// char* temp = new char[20];
		// _itoa_s(num, temp, 20, 10);
		// msg(temp);
		//delete []temp;

	std::stringstream numstream;
	numstream << num;
    char* temp = (char*)numstream.str().c_str();
	msg(temp);

	// Did not work
	// 		std::string numstring = std::to_string(num);
    // 		char* temp = (char*)numstring.c_str();
	// 		msg(temp);
}