#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "label.h"
#include "word.h"

// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using ASM::Token;
using ASM::Lexer;


int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  string err = "ERROR";
  vector< vector<Token*> > tokLines;
  Label labeltable;
  Word wordtable;
  string dis;
  int l = 0;
  int value;
  int m = 0;
  string colon = ":";
  vector < vector <Token *> > word;
  ASM::Kind k;
  const int jr = 8;
  const int jalr = 9;
  string s1;
  const int add = 32;
  const int sub = 34;
  const int slt = 42;
  const int sltu = 43;
  const int mfhi = 16;
  const int mflo = 18;
  const int lis = 20;
  const int mult = 24;
  const int multu = 25;
  const int div = 26;
  const int divu = 27;
  const int lw = 35;
  const int sw = 43;
  const int jump2 = 0x10000002;
  const int relocation = 0x01;
  int s = 0;
  int t = 0;
  int d = 0;
  int curadd = 0;
  vector <int> reltable;
  int address = 12;
  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }

    // Iterate over the lines of tokens and print them
    // to standard error
    string str;
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
      bool dotword = false;
      bool havenum = false;
	bool label = false;
	bool haveregister = false;
	bool havejump = false;
	bool haveadd = false;
	int number = 0;
	bool needcomma = false;
	bool havebranch = false;
	bool havelis = false;
	bool havemult = false;
	bool rparen = false;
	bool lparen = false;
	bool havelw = false;
	vector <Token *> temp;
	temp.resize(0);
      for(it2 = it->begin(); it2 != it->end(); ++it2) {
        k = (*it2)->getKind();
        if (k == ASM::DOTWORD) {
            if (!havelw && !havemult && !havelis && !haveadd && !dotword && !havejump && !havebranch) {dotword = true; address += 4;}
            else {cerr<<err<<endl;goto exit;}}
        else if (k == ASM::INT) {
            if ((!havenum && dotword) || (!havenum && havebranch && (number == 2) && !needcomma))
			{havenum=true; needcomma = false; temp.push_back(*it2);}
	    else if (!havenum && havelw && !needcomma && (number == 1)) {
		havenum = true; lparen = true; temp.push_back(*it2);}
            else {cerr<<err<<endl; goto exit;}}
        else if (k == ASM::HEXINT) {
            if ((!havenum&& dotword)||(!havenum && havebranch && (number == 2) && !needcomma)) 
		{ havenum = true; needcomma = false;temp.push_back(*it2);}
            else if (!havenum && havelw && !needcomma && (number == 1)) {
		havenum = true; lparen = true; temp.push_back(*it2);}
	    else {cerr<<err<<endl;goto exit;}}
        else if (k == ASM::ID) {
		 s1 = (*it2)->getLexeme();
		if (dotword && !havenum) 
			{havenum = true; reltable.push_back(address-4); temp.push_back(*it2);}
		else if (havebranch && !havenum && (number == 2) && !needcomma) 
			 {havenum = true; temp.push_back(*it2);}
		//add opcode
		else if (!havelw && !havemult && !havejump && !haveadd && !dotword && !havebranch && !havelis) {
 			if (s1 == "jr" || s1 == "jalr") {
				havejump = true; temp.push_back(*it2); address += 4;}
			else if (s1 == "add" || s1 == "sub" || s1 == "slt" || s1=="sltu") {
				haveadd = true; temp.push_back(*it2);address +=4;}
			else if (s1 == "beq" || s1 == "bne") {
				havebranch = true; temp.push_back(*it2); address += 4;}
			else if (s1 == "mfhi" || s1 == "mflo" || s1 == "lis") {
				havelis = true; temp.push_back(*it2); address += 4;}
			else if (s1 == "mult" || s1=="multu" || s1 == "div" || s1 == "divu") {
				havemult = true; temp.push_back(*it2);address += 4;}
			else if (s1 == "lw" || s1 == "sw") {
				havelw = true; temp.push_back(*it2); address += 4;}
			else {cerr<<err<<endl; goto exit;}}
		else {cerr<<err<<endl;goto exit;}}
        else if (k == ASM::WHITESPACE) {}
        else if (k == ASM::LABEL) {
	 	str = (*it2)->getLexeme();
		if (!dotword && !havejump) {
			if (labeltable.findLabel(str) == -1) {label = true;labeltable.addLabel(str,address);}
	   		else {cerr<<err<<endl; goto exit;}}
		else {cerr<<err<<endl; goto exit;}}
	else if (k == ASM::REGISTER) {
		if (!haveregister && havejump) {haveregister = true; temp.push_back(*it2);}
		else if (haveadd && (number < 3) && !needcomma) { 
			++number; 
			if (number == 3) {needcomma = false;}
			else {needcomma = true;}; temp.push_back(*it2);}
		else if ((havebranch && (number < 2) && !needcomma)||
			(havemult && (number < 1) && !needcomma)) {
			++number; needcomma = true; temp.push_back(*it2);}
		else if (havemult && (number == 1) && !needcomma) {
			++number; needcomma = false; temp.push_back(*it2);}
		else if (havelis && !haveregister) {
			haveregister = true; temp.push_back(*it2);}
		else if (havelw && (number == 0)) {
			number++; needcomma = true;temp.push_back(*it2);}
		else if (havelw && (number == 1) && !lparen) {
			number++; rparen = true; temp.push_back(*it2);}
		else {cerr<<err<<endl; goto exit;}}
	else if (k == ASM::COMMA) {
		if (needcomma) {needcomma = false;}
		else {cerr<<err<<endl; goto exit;}}
	else if (k == ASM::LPAREN) {
		if (lparen) {lparen = false;}
		else {cerr<<err<<endl; goto exit;}}
	else if (k == ASM::RPAREN) {
		if (rparen) {rparen = false;}
		else {cerr<<err<<endl; goto exit;}}
        else {cerr<<err<<endl;goto exit;};
  	++it2;
	if (it2 == it->end()) {if ((havelw && (number == 2) && havenum && !rparen) || (havelis || haveregister)||(havebranch && (number == 2) && havenum) || (havenum && dotword) || 
                                  (label && !dotword && !havejump && !haveadd && !havebranch) || 
					(haveregister && havejump) || ((number == 3) && haveadd) || ((number == 2) && havemult)) 
					{if (temp.size() != 0) {word.push_back(temp);};} 
				else {cerr<<err<<endl;goto exit;}}
	--it2;

       // cerr << "  Token: "  << *(*it2) << endl;
      }
    
   }
  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl; goto exit;
  }
  //print

// realocation
	wordtable.storeWord(jump2);
	int endmodule;
	endmodule = address + (reltable.size()*8);
//  cout << "end: "<<endmodule<<endl;
	wordtable.storeWord(endmodule);
 // cout << "endcode: "<<address<<endl;
	wordtable.storeWord(address);

  while (l < word.size()) {
	// for dotword
	if (word[l].size() == 1) {
		k = word[l][0]->getKind();
        	if (k == ASM::INT || k ==  ASM::HEXINT) 
			{value = word[l][0]->toInt(); wordtable.storeWord(value);}
		else  
			{ dis = word[l][0]->getLexeme(); dis += colon; value = labeltable.findLabel(dis);
		 	if (value == -1) {cerr <<err<<endl; goto exit;}
		 	else {wordtable.storeWord(value);}}}
	// for jr and jalr
	else if (word[l].size() == 2) {
		s1 = word[l][0]->getLexeme();
		if (s1 == "jr" || s1 == "jalr") 
		 	{value = word[l][1]->toInt(); 
			if (s1 == "jr") {value = ((value << 21) | jr);} 
			else {value = ((value << 21)|jalr);}; 
			wordtable.storeWord(value);}
		else if (s1 == "lis" || s1 == "mflo" || s1 == "mfhi") {
			d = word[l][1]->toInt();
			if (s1 == "lis") {value = ((d<<11)|lis); }
			else if (s1 == "mflo") {value = ((d<<11)|mflo);}
			else {value = ((d<<11) | mfhi);}
			wordtable.storeWord(value);}}
	else if (word[l].size() == 3) {
		s1 = word[l][0]->getLexeme();
		s = word[l][1]->toInt();
		t = word[l][2]->toInt();
		if (s1 == "mult") {value = mult;}
		else if (s1 == "multu") {value = multu;}
		else if (s1 == "div") {value = div;}
		else {value = divu;};
		value = (s<<21)|(t<<16)|value;
		wordtable.storeWord(value);}
	// for add sub ..
	else if (word[l].size() == 4) {
		s1 = word[l][0]->getLexeme();
		if (s1 == "add" || s1 == "sub" || s1 == "slt" || s1 == "sltu") {
			if (s1 == "add") {value = add;}
			else if (s1 == "sub") {value = sub;}
			else if (s1 == "slt") {value = slt;}
			else {value = sltu;};
			d = word[l][1]->toInt();
			s = word[l][2]->toInt();
			t = word[l][3]->toInt();
			value = (d<<11)|(t<<16)|(s<<21)|value;
			wordtable.storeWord(value);}
		else if (s1 == "beq" || s1 == "bne") {
			if (s1 == "beq") {value = 4;}
			else {value = 5;}
			s = word[l][1]->toInt();
			t = word[l][2]->toInt();
			k = word[l][3]->getKind();
			if (k == ASM::INT || k == ASM::HEXINT) {
				m = word[l][3]->toInt(); }
			else {
				dis = word[l][3]->getLexeme(); dis += colon; m = labeltable.findLabel(dis);
				if (m == -1) {cerr<<err<<endl;goto exit;}
				else {curadd = l * 4 + 12; m = (m - curadd - 4) / 4;}}
			// give m boundary;
			if (m < 0) {
				if (m < -65536) {
				cerr<<err<<endl; goto exit;}
				else {
				value = (value << 26);
				m = 0x0000ffff & m;
				value = (t<<16)|(s<<21)|m|value;
				wordtable.storeWord(value);}}	
				
			else {   
				if (m > 65535) {
				cerr<<err<<endl;}
				else {value = (value << 26);
				 value += m;
        			 value = (t<<16)|(s<<21)|value;
				wordtable.storeWord(value);}}}
		else if (s1 == "lw" || s1 == "sw") {
			t = word[l][1]->toInt();
			m = word[l][2]->toInt();
			s = word[l][3]->toInt();
			if (s1 == "lw") {value = lw;}
			else {value = sw;};
			if (m < 0) {
				if (m < -65536) {
					cerr<<err<<endl; goto exit;}
				else {
					value = (value << 26);
					m = 0x0000ffff & m;
					value = (t << 16)|(s << 21)|m|value;
					wordtable.storeWord(value);}}
			else {
				if (m > 65535) {
					cerr<<err<<endl;}
				else {value = value<<26; value += m; value = (t<<16)|(s<<21)|value;
					wordtable.storeWord(value);}}} }
			
	l++;}; 
 // cout<<"print label" <<endl;
  labeltable.printLabel();
 //relocation
 for (m = 0; m < reltable.size(); m++) {
	wordtable.storeWord(relocation);
	wordtable.storeWord(reltable[m]);
 };

 // cout<<"print binary"<<endl;
  wordtable.toBinary();
  exit:
  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }
 }
