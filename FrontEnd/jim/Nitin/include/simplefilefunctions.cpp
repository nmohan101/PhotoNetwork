jimstring SimpleFileFunctions::getcurrentworkingpath(int maxsize) {

	jimstring retrn(maxsize);
	bool success = false;

	#ifdef WINDOWS
		success = _getcwd(retrn.use(), retrn.len);
	#elif __linux__
		success = getcwd(retrn.use(), retrn.len);
	#endif

	if (success) {

		retrn.truncate();
	}
	else {

		retrn.fail();
	}

	return retrn;
}

jimstring SimpleFileFunctions::getcurrentpath(int maxsize) {

	jimstring retrn;

	jimstring fullpath(maxsize);

	ssize_t len = readlink("/proc/self/exe", fullpath.use(), maxsize - 1);

	if (len > -1) {

		fullpath.use()[len] = '\0';
		retrn = dirname(fullpath);
	}
	else {

		retrn.fail();
	}

	return retrn;
}

jimstring SimpleFileFunctions::readfile(const char* path) {

	jimstring retrn;

	simplefile infile;

	if (infile.openforread(path)) {

		infile.toend();

		__int64 filesize = infile.getpos();

		infile.tostart();

		//ToDo: Use 64 bit jimstring so that big files work. but maybe not, should probably stream those

		retrn.resize((int)filesize);

		infile.read(retrn.use(), filesize);

		infile.close();
	}
	else {

		retrn.fail();
	}

	return retrn;
}
