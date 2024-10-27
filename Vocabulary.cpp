// Vocabulary.cpp:                                        HDO, 2004-2016
// --------------
// Generic class Vocabulary shall be instantiated with [T|NT]Symbol only.
// An object of an instance of the generic class Vocabulary is a set of
// pointers to (possibly different types of) symbols: V = VT + VNT:
// Vocabulary.cpp would not be necessary, but this one provides
// a simple test program.
// =====================================================================

#include "Vocabulary.h"

// nothing to implement for generic class Vocabulary: the header is sufficient


// === test ============================================================

#if 0

#include <cstdio>

#include <iostream>
#include <typeinfo>

using namespace std;

#include "SymbolStuff.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  SymbolPool *sp = new SymbolPool();

  Vocabulary< TSymbol> vT;
  Vocabulary<NTSymbol> vNt;

    Symbol *  sy = nullptr;
   TSymbol * tSy = sp-> tSymbol("t");
  NTSymbol *ntSy = sp->ntSymbol("NT");

  vT .insert(tSy);
  vNt.insert(ntSy);

  cout << "vT:  " << vT  << endl;
  cout << "vNt: " << vNt << endl;

  cout << "vT .contains(t): " << vT. contains( tSy) << endl;
  cout << "vNt.contains(NT): " << vNt.contains(ntSy) << endl;

  Vocabulary< TSymbol> vT2(vT); // same as ... vT2 = vT;
  cout << "vT2: " << vT2 << endl;
  Vocabulary< TSymbol> vT3;
  vT3 = vT;
  cout << "vT3: " << vT3 << endl;

  string name;

  name = "t";
  cout << "vT .symbolFor(\"" << name << "\"): ";
  sy = vT.symbolFor(name);
  if (sy != nullptr)
    cout << *sy;
  else
    cout << "nullptr";
  cout << endl;

  name = "NT";
  cout << "vNt.symbolFor(\"" << name << "\"): ";
  sy = vNt.symbolFor(name);
  if (sy != nullptr)
    cout << *sy;
  else
    cout << "nullptr";
  cout << endl;

  Vocabulary<Symbol> v;
  for (auto sy: vT)
    v.insert(sy);
  for (auto sy: vNt)
    v.insert(sy);
  cout << "v: " << v << endl;

  delete sp;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of Vocabulary.cpp
// =====================================================================
