unsigned char SimpleString::convertchartohex(unsigned char thevalue) {

	return thevalue + (thevalue > '\t' ? '7' : '0');
}

unsigned char SimpleString::converthextochar(unsigned char thehexdigit) {

	return thehexdigit - (thehexdigit <= '9' ? '0' : '7');
}