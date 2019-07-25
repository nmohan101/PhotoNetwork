
simplefile::simplefile() {
	thestream = 0;
}

bool simplefile::openforwrite(const char* path) {

	if (thestream) {

		msg("SimpleCoding Runtime Error: simplefile::openforwrite() file already open");

		return false;
	}

#ifdef _WIN32
	if (fopen_s(&thestream, path, "wb")) {
#elif __linux__
	thestream = fopen(path, "wb");
	if (!thestream) {
#endif

		return false;
	}
    return true;
}
bool simplefile::openforread(const char* path) {

	if (thestream) {

		msg("SimpleCoding Runtime Error: simplefile::openforread() file already open");

		return false;
	}

#ifdef _WIN32
	if (fopen_s(&thestream, path, "rb")) {
#elif __linux__
	thestream = fopen(path, "rb");
	if (!thestream) {
#endif

		return false;
	}

    return true;
}
bool simplefile::openforboth(const char* path) {

	if (thestream) {

		msg("SimpleCoding Runtime Error: simplefile::openforboth() file already open");

		return false;
	}

#ifdef _WIN32
	if (fopen_s(&thestream, path, "r+b")) {
#elif __linux__
	thestream = fopen(path, "r+b");
	if (!thestream) {
#endif

		return false;
	}

    return true;
}
bool simplefile::openfreshforboth(const char* path) {

	if (thestream) {

		msg("SimpleCoding Runtime Error: simplefile::openfreshforboth() file already open");

		return false;
	}

#ifdef _WIN32
	if (fopen_s(&thestream, path, "w+b")) {
#elif __linux__
	thestream = fopen(path, "w+b");
	if (!thestream) {
#endif

		return false;
	}
    return isopen();
}

template <class type> bool simplefile::save(const char* filename, const type& info) {

	simplefile outfile;

	if (outfile.openforwrite(filename)) {

		outfile << info;

		outfile.close();

		return true;
	}

	return false;
}
template <class type> bool simplefile::save(const char* filename, const jimhash& info, const type& stupidtype) {

	simplefile outfile;

	if (outfile.openforwrite(filename)) {

		info.save(outfile, type());

		outfile.close();

		return true;
	}

	return false;
}

template <class type> bool simplefile::load(const char* filename, type& info) {

	simplefile infile;

	if (infile.openforread(filename)) {

		info >> info;

		infile.close();

		return true;
	}

	return false;
}
template <class type> bool simplefile::load(const char* filename, jimhash& info, const type& stupidtype) {

	simplefile infile;

	if (infile.openforread(filename)) {

		info.load(infile, type());

		infile.close();

		return true;
	}

	return false;
}

bool simplefile::isopen() {

    if (thestream) {

		return true;
	}
    else {

		return false;
	}
}

bool simplefile::eof() {

	return feof(thestream) != 0;
}

void simplefile::close() {

	if (!thestream) {

		msg("SimpleCoding Runtime Error: simplefile::close() file not open");
	}

	fclose(thestream);
	thestream = 0;
}

void simplefile::flush() {

	if (!thestream) {

		msg("SimpleCoding Runtime Error: simplefile::flush() file not open");
	}

	fflush(thestream);
}
template<class type> bool simplefile::write(type* info, int size) {

	if (!thestream) {

		msg("SimpleCoding Runtime Error: simplefile::write() file not open");
		return false;
	}

	if (!size) {

		return true;
	}

	if (!fwrite((void*)info, sizeof(type), size, thestream)) {

		return false;
	}

	return true;
}
template <class type> bool simplefile::write(type& info) {

	return write(&info, 1);
}
template<class type> bool simplefile::read(type* info, int size) {

	if (!thestream) {

		msg("SimpleCoding Runtime Error: simplefile::read() file not open");
		return false;
	}

	if (!size) {

		return true;
	}

	if (!fread((void*)info, sizeof(type), size, thestream)) {

		return false;
	}

	return true;
}
template <class type> bool simplefile::read(type& info) {

	return read(&info, 1);
}

simplefile& simplefile::operator<<(char* info) {write(info, strlen(info)); return *this;}
simplefile& simplefile::operator<<(char info) {write(info); return *this;}
simplefile& simplefile::operator>>(char& info) {read(info); return *this;}
simplefile& simplefile::operator<<(unsigned char info) {write(info); return *this;}
simplefile& simplefile::operator>>(unsigned char& info) {read(info); return *this;}
simplefile& simplefile::operator<<(int info) {write(info); return *this;}
simplefile& simplefile::operator>>(int& info) {read(info); return *this;}
simplefile& simplefile::operator<<(float info) {write(info); return *this;}
simplefile& simplefile::operator>>(float& info) {read(info); return *this;}
simplefile& simplefile::operator<<(double info) {write(info); return *this;}
simplefile& simplefile::operator>>(double& info) {read(info); return *this;}
simplefile& simplefile::operator<<(__int64 info) {write(info); return *this;}
simplefile& simplefile::operator>>(__int64& info) {read(info); return *this;}


__int64 simplefile::getpos() {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::getpos file not open");

#ifdef _WIN32
	return _ftelli64(thestream);
#elif __linux__
	return ftello64(thestream);
#endif
}

void simplefile::setpos(const __int64& newpos) {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::setpos file not open");

#ifdef _WIN32
	_fseeki64(thestream, newpos, SEEK_SET);
#elif __linux__
	fseeko64(thestream, newpos, SEEK_SET);
#endif
}

void simplefile::move(const __int64& num) {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::move file not open");

#ifdef _WIN32
	_fseeki64(thestream, num, SEEK_CUR);
#elif __linux__
	fseeko64(thestream, num, SEEK_CUR);
#endif
}
void simplefile::tostart() {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::tostart file not open");

#ifdef _WIN32
	_fseeki64(thestream, 0, SEEK_SET);
#elif __linux__
	fseeko64(thestream, 0, SEEK_SET);
#endif
}
void simplefile::toend() {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::toend file not open");

#ifdef _WIN32
	_fseeki64(thestream, 0, SEEK_END);
#elif __linux__
	fseeko64(thestream, 0, SEEK_END);
#endif
}

void simplefile::ensuresize(const __int64& size) {

	if (!thestream) {

		msg("SimpleCoding Runtime Error: simplefile::ensuresize file not open");
	}

	__int64 position = getpos();
	setpos(size - 1);
	write("0", 1);
	setpos(position);
}

void simplefile::matrixheaderend(int width, int height, int unitsize, bool inverty, int paddingmodulus) {


	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::matrixheaderend file not open");

	matrixwidth = width;
	matrixheight = height;
	matrixunitsize = unitsize;
	matrixheadersize = (int)getpos();
	matrixinverty = inverty;
	matrixpaddingmodulus = paddingmodulus;
}

template <class type> bool simplefile::matrixwrite(int x, int y, type* data, int size) {


	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::matrixwrite file not open");

	if (matrixinverty)
		y = matrixheight - y - 1;

	__int64 position = (
		matrixheadersize
		+ (
			(__int64)(matrixwidth + (matrixwidth % matrixpaddingmodulus)) * y
			+ x
		) * matrixunitsize
	);

	setpos(position);
	return write(data, size);
}

template <class type> bool simplefile::matrixread(int x, int y, type* data, int size) {

	if (!thestream)
		msg("SimpleCoding Runtime Error: simplefile::matrixwrite file not open");

	if (matrixinverty)
		y = matrixheight - y - 1;

	__int64 position = (
		matrixheadersize
		+ (
			(__int64)(matrixwidth + (matrixwidth % matrixpaddingmodulus)) * y
			+ x
		) * matrixunitsize
	);

	setpos(position);
	return read(data, size);
}
