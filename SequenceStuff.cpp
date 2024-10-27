// SequenceStuff.cpp:                                     HDO, 2004-2020
// -----------------
// Classes Sequence and SequenceSet for sets of Sequence objects.
// Sequence objects represent (possibly empty) sequences of
//   pointers to either T- or NTSymbol objects.
// SequenceSet objects take ownership of their sequences.
// =====================================================================

#include <cstdarg>

#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

#include "SequenceStuff.h"


void checkForNullptr(void *ptr, const char *errMsg) {
  if (ptr == nullptr)
    throw invalid_argument(errMsg);
} // checkForNullptr


// === implementation of class Sequence ================================

Sequence::Sequence(Symbol *sy) {
  checkForNullptr(sy, "invalid nullptr for symbol");
  push_back(sy);
} // Sequence::Sequence

Sequence::Sequence(initializer_list<Symbol *> il) {
  for (auto sy: il) {
    checkForNullptr(sy, "invalid nullptr for symbol");
    push_back(sy);
  } // for
} // Sequence::Sequence

Sequence::Sequence(const std::string &str) {
  istringstream iss(str);
  string syName;
  SymbolPool sp;
  while (!iss.eof()) {
    syName.clear();
    iss >> syName;
    if (syName.length() > 0) {
      Symbol *sy = sp.symbolFor(syName);
      checkForNullptr(sy, ("invalid symbol name \"" + syName +
                           "\" in string").c_str());
      push_back(sy);
    } // if
  } // while
} // Sequence::Sequence

template<typename ItT>
Sequence::Sequence(ItT begin, ItT end) {
  for (ItT it = begin; it != end; it++) {
    checkForNullptr(*it, "invalid nullptr for symbol");
    push_back(*it);
  } // for
} //Sequence::Sequence


void Sequence::check(int idx) const {
  if ( (0 <= idx) && (idx < length()) )
    return;
  throw range_error("sequence index out of range");
} // Sequence::check

void Sequence::check(iterator it) const {
  if ( (begin() <= it) && (it < end()) )
    return;
  throw range_error("sequence iterator out of range");
} // Sequence::check

int Sequence::length() const {
  return static_cast<int>(size());
} // Sequence::length

int Sequence::terminalLength() const {
  int n = 0;
  for (auto sy: *this)
    if (sy->isT())
      n++;
  return n;
} // Sequence::termnialLength

void Sequence::append(Symbol *sy) {
  checkForNullptr(sy, "invalid nullptr for symbol");
  push_back(sy);
} // Sequence::append

void Sequence::append(Sequence *seq) {
  checkForNullptr(seq, "invalid nullptr for sequence");
  for (auto sy: *seq) {
    append(sy);
  } // for
} // Sequence::append

Symbol *&Sequence::operator[](int idx) {
  check(idx);
  return Base::operator[](static_cast<Base::size_type>(idx));
} // Sequence::operator[]

Symbol *Sequence::symbolAt(int idx) const {
  check(idx);
  return *(begin() + idx);
} // Sequence::symbolAt

Symbol *Sequence::symbolAt(iterator it) const {
  check(it);
  return *it;
} // Sequence::symbolAt

void Sequence::removeSymbolAt(int      idx) {
  check(idx);
  erase(begin() + idx);
} // removeSymbolAt

void Sequence::removeSymbolAt(iterator it) {
  check(it);
  erase(it);
} // removeSymbolAt

void Sequence::replaceSymbolAt(int idx, Symbol *sy) {
  check(idx);
  replaceSymbolAt(begin() + idx, sy); // see below
} // Sequence::replaceSymbolAt

void Sequence::replaceSymbolAt(iterator it, Symbol *sy) {
  check(it);
  checkForNullptr(sy, "invalid nullptr for symbol");
  *it = sy;
} // Sequence::replaceSymbolAt

void Sequence::replaceSymbolAt(int idx, Sequence *seq) {
  check(idx);
  replaceSymbolAt(begin() + idx, seq); // see below
} // Sequence::replaceElementAt

void Sequence::replaceSymbolAt(iterator it, Sequence *seq) {
  check(it);
  checkForNullptr(seq, "invalid nullptr for sequence");
  it = erase(it);
  if (seq->size() > 0)
    insert(it, seq->begin(), seq->end());
} // Sequence::replaceElementAt

bool Sequence::hasTerminalsOnly() const {
  for (const Symbol *sy: *this)
    if (sy->isNT())
      return false;
  return true;
} // Sequence::hasTerminalsOnly

bool Sequence::isEpsilon() const {
  return size() == 0;
} // Sequence::isEpsilon


bool operator<(const Sequence &seq1, const Sequence &seq2) { // lexicographically
  if (&seq1 == &seq2) // identical, so ==
    return false;
  Sequence::const_iterator it1 = seq1.begin();
  Sequence::const_iterator it2 = seq2.begin();
  while ( (it1 != seq1.end()) &&
          (it2 != seq2.end()) ) {
    int cmpRes = (*it1)->name.compare((*it2)->name);
    if (cmpRes != 0)
      return (cmpRes < 0);
    // cmpRes == 0, so continue
    it1++;
    it2++;
  } // while
  return ((it1 == seq1.end()) &&
          (it2 != seq2.end()));
} // operator<

bool operator==(const Sequence &seq1, const Sequence &seq2) {
  if (&seq1 == &seq2) // identical, so ==
    return true;
  if (seq1.length() != seq2.length())
    return false;
  // same size, so compare elements
  return equal(seq1.begin(), seq1.end(),
               seq2.begin(),
               EqualForSymbolPtrs());
} // operator==


bool lexLessForSequencePtrs(const Sequence* seq1, const Sequence* seq2) {
  return (*seq1) < (*seq2);
} // lexLessForSequencePtrs

bool lenLexLessForSequencePtrs(const Sequence* seq1, const Sequence* seq2) {
  int len1 = seq1->length();
  int len2 = seq2->length();
  if (len1 != len2)
   return len1 < len2;
  // len1 == len2, same size, so compare elements
  for (int i = 0; i < len1 /*or len2*/; i++) {
    int cmpRes = const_cast<Sequence &>(*seq1)[i]->name.compare(
                 const_cast<Sequence &>(*seq2)[i]->name);
    if (cmpRes == 0) // names are equal
      continue;
    return (cmpRes < 0); // for different names
  } // for
  return false; // seq1 and seq2 are equal in length and contents
} // lenLexLessForSequencePtrs


bool equalForSequencePtrs(const Sequence *seq1, const Sequence *seq2) {
  return (*seq1) == (*seq2);
} // equalForSequencePtrs


ostream &operator<<(ostream &os, const Sequence &seq) {
  if (seq.isEpsilon())
    os << "eps";
  else {
    bool first = true;
    for (const Symbol *sy: seq) {
      if (!first)
        os << " ";
      os << *sy;
      first = false;
    } // for
  } // else
  return os;
} // operator<<


// === implementation of class SequenceSet =============================

SequenceSet::SequenceSet()
: Base(lexLessForSequencePtrs) {
  // nothing left do do
} // SequenceSet::SequenceSet

SequenceSet::SequenceSet(const SequenceSet &ss)
: Base(lexLessForSequencePtrs) /*OC+*/ , ObjectCounter<SequenceSet>() /*+OC*/ {
  // make a deep copy: init. Base with copies of elements in ss
  for (auto &seq: ss) {
    insert(new Sequence(*seq));
  } // for
} // SequenceSet::SequenceSet

SequenceSet::SequenceSet(Sequence *s)
: Base(lexLessForSequencePtrs) {
  checkForNullptr(s, "invalid nullptr for sequence");
  insert(s);
} // SequenceSet::SequenceSet

SequenceSet::SequenceSet(std::initializer_list<Sequence *> il)
: Base(lexLessForSequencePtrs) {
  for (Sequence *s: il) {
    checkForNullptr(s, "invalid nullptr for sequence");
    cout << "inserting " << *s << endl;
    insert(s);
  } // for
} // SequenceSet::SequenceSet


SequenceSet::SequenceSet(SequencePtrCmp seqPtrCmp)
: Base(seqPtrCmp) {
  checkForNullptr((void*)seqPtrCmp, "invalid nullptr for sequence comparison");
} // SequenceSet::SequenceSet


SequenceSet::~SequenceSet() {
  for (auto &seq: *this) {
    delete seq;
  } // for
} // SequenceSet::~SequenceSet


void SequenceSet::insertOrDelete(Sequence *&s) {
  checkForNullptr(s, "invalid nullptr for sequence");
  auto ir = insert(s);
  if (!ir.second) { // s has not been inserted, so it's a duplicate
    delete s;
    s = nullptr;
  } // if
} // SequenceSet::insertOrDelete


bool operator==(const SequenceSet &ss1, const SequenceSet &ss2) {
  if (&ss1 == &ss2)
    return true;
  if (ss1.size() != ss2.size())
    return false;
  return equal(ss1.begin(), ss1.end(),
               ss2.begin(),
               equalForSequencePtrs);
} // operator==

ostream &operator<<(ostream &os, const SequenceSet &ss) {
  os << "{ ";
  bool first = true;
  for (auto &seq: ss) {
    if (!first)
      os << ", ";
    os << *seq;
    first = false;
  } // for
  os << " }" << endl;
  return os;
} // operator<<


// === test ============================================================

#if 0

#include <cstdio>
#include <typeinfo>

#include "SymbolStuff.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: SequenceStuff test" << endl;
  cout << endl;

  SymbolPool *sp = new SymbolPool();

  Sequence *eps = new Sequence();
  cout << "eps = " << *eps << endl;

   TSymbol *tSy  = sp->tSymbol("terminal");
   TSymbol *aSy  = sp->tSymbol("a");
   TSymbol *bSy  = sp->tSymbol("b");
  NTSymbol *ntSy = sp->ntSymbol("nonterminal");
  NTSymbol *ASy  = sp->ntSymbol("A");
  NTSymbol *BSy  = sp->ntSymbol("B");

  Sequence *seq1    = new Sequence(tSy);
  seq1->append(ntSy);

  Sequence *seq1a   = new Sequence(seq1->begin(), seq1->end());

  Sequence *seq2    = new Sequence({ntSy, tSy});
  Sequence *seq2a   = new Sequence("nonterminal terminal");

  Sequence *seq2b   = new Sequence(*seq2);

  cout << "seq1  = " << *seq1  << endl;
  cout << "seq1a = " << *seq1a << endl;
  cout << "seq2  = " << *seq2  << endl;
  cout << "seq2a = " << *seq2a << endl;
  cout << "seq2b = " << *seq2b << endl;

  seq1->append(seq2);
  cout << "seq1+seq2, new seq1 = " << *seq1 << endl;

  seq1->removeSymbolAt(0);
  cout << "new seq1 w.o. first = " << *seq1 << endl;

  cout << endl;

  delete seq1;
  delete seq1a;
  delete seq2;
  delete seq2a;
  delete seq2b;

  seq1 = new Sequence(  {aSy, ASy});
  seq2 = new Sequence(  {aSy, BSy});

  cout << "seq1 = " << *seq1 << endl;
  cout << "seq2 = " << *seq2 << endl;

  cout << "seq1 <  seq2 == " << (*seq1 <  *seq2) << endl;
  cout << "lexLessForSequencePtrs(seq1,  seq2) == " <<
           lexLessForSequencePtrs(seq1, seq2) << endl;
  cout << "seq1 == seq2 == " << (*seq1 == *seq2) << endl;
  cout << "seq1 >  seq2 == " << (*seq1 >  *seq2) << endl;

  cout << endl;
  Sequence *seq3 = new Sequence(bSy);
  Sequence *seq4 = new Sequence({aSy, bSy});
  Sequence *seq5 = new Sequence({bSy, aSy});
  cout << "seq3 = " << *seq3 << endl;
  cout << "seq4 = " << *seq4 << endl;
  cout << "seq5 = " << *seq5 << endl;

  cout << "lenLexLessForSequencePtrs(seq3,  seq4) == " <<
           lenLexLessForSequencePtrs(seq3, seq4) << endl;
  cout << "lenLexLessForSequencePtrs(seq4,  seq5) == " <<
           lenLexLessForSequencePtrs(seq4, seq5) << endl;
  cout << "lenLexLessForSequencePtrs(seq5,  seq4) == " <<
           lenLexLessForSequencePtrs(seq5, seq4) << endl;

  cout << endl;

  SequenceSet *ss1 = new SequenceSet;
  ss1->insert(new Sequence());
  ss1->insert(seq1);
  ss1->insert(seq2);

  Sequence *seq6 = new Sequence(*seq2);
  cout << "seq6 is duplicate of seq2 = " << *seq6 << endl;
  ss1->insertOrDelete(seq6); // a duplicate
  if (seq6 != nullptr)
    cerr << "ERROR: seq6 should have been deleted" << endl;

  cout << endl;
  cout << "sequence set ss1 = " << *ss1;

  SequenceSet *ss2 = new SequenceSet(*ss1);
  cout << "copy of sequence set ss2 = " << *ss2;
  cout << "original == copy ?: " << (*ss1 == *ss2) << endl;

  cout << endl;
  SequenceSet *ss3 = new SequenceSet(new Sequence(*seq1));
  cout << "ss3 = " << *ss3 << endl;

  SequenceSet *ss4 = new SequenceSet({new Sequence(*seq1), new Sequence(*seq2)});
  ss4->insert(new Sequence(bSy));
  cout << "ss4 = " << *ss4 << endl;

  SequenceSet *ss5 = new SequenceSet(lenLexLessForSequencePtrs);
  ss5->insert(new Sequence(bSy));
  ss5->insert(new Sequence(*seq1));
  ss5->insert(new Sequence(*seq2));
  cout << "ss5 = " << *ss5 << endl;

  delete ss5;
  delete ss4;
  delete ss3;
  delete ss2;
  delete ss1;

  delete eps;
  // delete seq1;  // already deleted by ss1
  // delete seq2;  // already deleted by ss2

  delete seq3;
  delete seq4;
  delete seq5;



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

// end of SequenceStuff.cpp
//======================================================================
