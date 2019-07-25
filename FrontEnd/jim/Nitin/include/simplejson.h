class SimpleJSON {
public:

	bool isnumberchar(char thechar);
	int specialchars(jimstring& text, int& position, jimvector<int>& positions);

	jimstring getstring(jimstring& text, int& position, jimvector<int>& positions);
	float getnumber(jimstring& text, int& position, jimvector<int>& positions);
	jimvector<jimitem> getlist(jimstring& text, int& position, jimvector<int>& positions);
	jimhash getobject(jimstring& text, int& position, jimvector<int>& positions);
	jimitem& getvalue(jimstring& text, int& position, jimvector<int> &positions, jimitem& retrn);

	jimitem parse(const char* json);

	jimstring stringify(const jimitem& jsonitem, int prettytabcount = -1);
	jimstring stringify(const jimhash& jsonhash, bool pretty = false);
	jimstring stringify(const jimvector<jimitem>& jsonhash, bool pretty = false);
};

SimpleJSON simplejson;