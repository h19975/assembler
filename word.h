#ifndef _WORD_H_
#define _WORD_H_
#include <iostream>
#include <vector>

using std::vector;

class Word {
  vector <int> word;
  public:
  void storeWord(int i);
  void toBinary();
};
#endif
