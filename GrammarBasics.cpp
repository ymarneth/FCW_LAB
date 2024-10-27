// GrammarBasics.cpp:                                          HDO, 2020
// -----------------
// Basic types for classes GrammarBuilder and Grammar.
//======================================================================

#include <sstream>
using namespace std;

#include "GrammarBasics.h"


// === implementation of class RulesMap ================================

const SequenceSet RulesMap::constEmptyElement = SequenceSet();

// non-inserting operator[] for const RulesMap objects
const SequenceSet &RulesMap::operator[](NTSymbol *ntSy) const {
  auto it = Base::find(ntSy);
  if (it != Base::end()) // an element (rule) exists
    return it->second;
  else
    return constEmptyElement;
} // RulesMap::operator[]


// === test ============================================================

#if 0

#include <cstdio>

#include <typeinfo>

#include "SymbolStuff.cpp"
#include "SequenceStuff.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: GrammarBasics test" << endl;
  cout << endl;

  SymbolPool *sp = new SymbolPool();

  // build simple RulesMap for
  // S -> EPS | A
  // A -> a | A a

  NTSymbol *S = sp->ntSymbol("S");
  NTSymbol *A = sp->ntSymbol("A");
  NTSymbol *X = sp->ntSymbol("X");
  TSymbol  *a = sp-> tSymbol("a");

  VNt vNt; vNt.insert(S); vNt.insert(A);
  VT  vT;  vT.insert(a);
  V   v;   v.insert(S); v.insert(A); v.insert(a);

  cout << "vocabularies:" << endl;
  cout << "  vNt = " << vNt << endl;
  cout << "  vT  = " << vT  << endl;
  cout << "  v   = " << v   << endl;

  cout << endl;

  Sequence *seq0 = new Sequence();
  Sequence *seq1 = new Sequence(A);
  Sequence *seq2 = new Sequence(a);
  Sequence *seq3 = new Sequence({a, A});

  cout << endl;
  cout << "RuleMaps:" << endl;
  RulesMap rm;
  rm.insert(make_pair(S, SequenceSet({seq0, seq1})));
  rm.insert(make_pair(A, SequenceSet({seq2, seq3})));

  cout << "rm:" << endl;
  for (auto &p: rm)
    cout << "  " << *p.first << " -> " << p.second;

  cout << endl;
  cout << "rm[S] = " << rm[S] << endl;
  cout << "rm[X] = " << rm[X] << endl;

  const RulesMap crm; // crm is empty!
  cout << "crm[S] = " << crm[S] << endl;

  delete sp;

  cout << endl;
  cout << "END" << endl;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of GrammarBasics.cpp
//======================================================================