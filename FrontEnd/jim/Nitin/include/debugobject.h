class debugobject {
public:

	jimstring value;

	debugobject();
	debugobject(char* text);
	debugobject(const debugobject& match);
	debugobject(debugobject&& match);

	~debugobject();

	debugobject& operator=(const debugobject& match);
	debugobject& operator=(debugobject&& match);
};