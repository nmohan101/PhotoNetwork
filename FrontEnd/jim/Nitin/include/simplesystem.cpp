
jimstring SimpleSystem::runcommand(const char* thecommand) {

	jimstring retrn;
	jimstring buf(256);


	jimstring commandstring(thecommand + _(" 2>/dev/null"));

	FILE *stream = popen(commandstring.use(), "r");

	if (stream) {

		try {

			while (!feof(stream)) {

				if (fgets(buf.use(), buf.len, stream)) {

					retrn += buf.use();
				}
			}
		}
		catch(...) {

			pclose(stream);
		}

		pclose(stream);
	}

	return retrn;
}

__int64 SimpleSystem::gettotalram() {

	return sysconf(_SC_PHYS_PAGES);
}

__int64 SimpleSystem::getfreeram() {

	return sysconf(_SC_AVPHYS_PAGES);
}

__int64 SimpleSystem::getusedram() {

	return gettotalram() - getfreeram();
}
