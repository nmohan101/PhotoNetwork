class jimitemlist : public jimvector<jimitem*> { public:
	
	jimitem& jimitemlist::operator[](int theindex) {
		return index(theindex);
	};

	jimitem& jimitemlist::index(int theindex) {
		
		if (theindex < 0)
			theindex += size;

		if (theindex > -1 && theindex <= size) {

			if (theindex == size) {

				addnum(1);
				array[theindex] = new jimitem();
			}

			return *array[theindex];
		}

		msg("SimpleCoding runtime error: index out of range on jimitemlist");
		jimitem *retrn = 0;
		return *retrn;
	};
};
