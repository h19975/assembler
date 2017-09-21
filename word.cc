#include "word.h"
#include <stdio.h>
#include <iostream>

using namespace std;


void Word::storeWord(int i) {
  word.push_back(i);
};

void Word::toBinary() {
 int i = 0;
   while (i < word.size()) {
      //debug
//        cout<<"hello"<<endl;
        putchar(word[i]>>24);
        putchar(word[i]>>16);
        putchar(word[i]>>8);
        putchar(word[i]);
        i++;}
};


