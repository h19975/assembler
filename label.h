#ifndef _LABEL_H_
#define _LABEL_H_
#include <map>
using std::string;
using std::map;

class Label {
	map <string,int> label;
	public:
	void addLabel(string str, int address);
        void printLabel();
	int findLabel(string key);
};

#endif
