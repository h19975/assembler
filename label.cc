#include "label.h"
#include <map>
#include <iostream>

using namespace std;


void Label::addLabel(string str, int address) {
	str.pop_back();
	label [str] = address;
}

void Label::printLabel() {
	for (auto &p:label) {
	cerr<<p.first<< " " <<p.second<<endl;
}
}

int Label::findLabel(string key) {
	key.pop_back();
	if (label.count(key)) {
 		return label [key];}
	else {return -1;}
}


	
	
	
