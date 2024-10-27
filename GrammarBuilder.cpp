// GrammarBuilder.cpp:                                         HDO, 2020
// ------------------
// GrammarBuilder implements the builder pattern in order to build
// Grammar objects.
//======================================================================


#include <cstring>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

#include "ObjectCounter.h"
#include "GrammarBasics.h"
#include "Grammar.h"
#include "GrammarBuilder.h"


// === implementation of class GrammarBuilder ==========================

void GrammarBuilder::initialize(NTSymbol *root) {
  this->root = root;
  insertIntoVNt(root);
  // rest of vNt and vT filled by rest of constructor or via addRule
} // GrammarBuilder::initialize


void GrammarBuilder::readGrammar(istream &is) {
  string line, sy, rootNt, nt, ntSy, arrowSy;
  bool firstNonEmptyLine;
  unordered_map<string, NTSymbol *> ntMap;
  unordered_map<string, TSymbol  *>  tMap;

  SymbolPool sp;

  // 1. read file and map nonterminals
  firstNonEmptyLine = true;
  while (!is.eof()) {
    line = "";
    getline(is, line);
    if (is.fail())
      break;
    sy = "";
    istringstream(line) >> sy;
    if (sy == "" || sy.substr(0, 2) == "//") // skip empty or comment line
      continue;
    if (sy.substr(0, 3) == "---") // start of opt. symbol information: skip
      break;
    if (firstNonEmptyLine) {     // sy should look like "G(...):"
      firstNonEmptyLine = false;
      if ((sy.substr(0, 2) != "G(") ||
        (sy.substr(sy.length() - 2, 2) != "):"))
        throw runtime_error("grammar does not start with \"G(...):\"");
      rootNt = sy.substr(2, sy.length() - 4);
      if ((rootNt == "") || (rootNt.length() > 20))
        throw runtime_error("invalid root nonterminal \"" + rootNt + "\"");
    } else {                     // additional non empty line, sy should be a nt
      if (ntMap[sy] != nullptr)
        throw runtime_error("duplicate nonterminal \"" + sy + "\"");
      ntMap[sy] = sp.ntSymbol(sy);
    } // else
  } // while

  // 2. initialize Grammar object according to default constructor
  root = ntMap[rootNt];
  if (root == nullptr)
    throw runtime_error("rule for root nonterminal \"" + rootNt +
      "\" missing");
  initialize(root);

  // 3. re-read istream and generate terminals, sequences and rules
  is.clear();                    // reset flags (especially eof and fail)
  is.seekg(0);                   // rewind to the beginning
  firstNonEmptyLine = true;
  Sequence *seq = nullptr;
  int lnr = 0;
  while (!is.eof()) {
    line = "";
    getline(is, line);
    if (is.fail())
      break;
    lnr++;
    istringstream ls(line);      // line stream
    sy = "";
    ls >> sy;
    if (sy == "" || sy.substr(0, 2) == "//")  // skip empty or comment line
      continue;
    if (sy.substr(0, 3) == "---") // start of opt. symbol information
      break;
    if (firstNonEmptyLine) {     // sy should be "G(...):"
      firstNonEmptyLine = false;
      continue;                  // skip
    } // if
    ntSy = sy;                   // non empty line starting with nt
    arrowSy = "";
    ls >> arrowSy;
    if (arrowSy != "->") {
      ostringstream lnrs;        // line number stream
      lnrs << lnr;
      throw runtime_error("syntax error in line " + lnrs.str() +
        ": -> missing");
    } // if
    if (seq == nullptr)
      seq = new Sequence();
    while (!ls.eof()) {
      sy = "";
      ls >> sy;
      if (sy == "|") {
        addRule(ntMap[ntSy], seq);
        seq = new Sequence();
      } else if (sy != "") { // sy != "|" && sy != ""
        if ((sy == "EPS") || (sy == "EPSILON") ||
          (sy == "eps") || (sy == "epsilon"))
          ; // nothing to do: seq is epsilon
        else if (ntMap[sy] != nullptr) // sy is a nonterminal
          seq->append(ntMap[sy]);
        else {                   // sy is a terminal
          if (tMap[sy] == nullptr)
            tMap[sy] = sp.tSymbol(sy);
          seq->append(tMap[sy]);
        } // else
      } // else
    } // while
    addRule(ntMap[ntSy], seq);
    seq = nullptr;
  } // while
} // GrammarBuilder::readGrammar


bool GrammarBuilder::insertIntoVNt(NTSymbol *ntSy) {
  Symbol *sy = v.symbolFor(ntSy->name);
  if (sy != nullptr) {
    if (sy->isT())
      throw invalid_argument("name clash for NT: a T already named \"" +
        ntSy->name + "\"");
    return false;
  } else { // sy == nullptr
    vNt.insert(ntSy);
    v.insert(ntSy);
    return true;
  } // else
} // GrammarBuilder::insertIntoVNt

bool GrammarBuilder::insertIntoVT(TSymbol *tSy) {
  Symbol *sy = v.symbolFor(tSy->name);
  if (sy != nullptr) {
    if (sy->isNT())
      throw invalid_argument("name clash for T: a NT already named \"" +
        tSy->name + "\"");
    return false;
  } else { // sy == nullptr
    vT.insert(tSy);
    v.insert(tSy);
    return true;
  } // else
} // GrammarBuilder::insertIntoVT


GrammarBuilder::GrammarBuilder(NTSymbol *root) {
  checkForNullptr(root, "invalid nullptr for root nonterminal");
  initialize(root);
} // GrammarBuilder::GrammarBuilder

GrammarBuilder::GrammarBuilder(const string &fileName) {
  ifstream ifs(fileName);
  if (!ifs.good())
    throw invalid_argument("file \"" + fileName + "\" not found");
  readGrammar(ifs);
} //GrammarBuilder::GrammarBuilder

GrammarBuilder::GrammarBuilder(const char *grammarStr) {
  istringstream iss(string(grammarStr, strlen(grammarStr)));
  readGrammar(iss);
} // GrammarBuilder::GrammarBuilder


bool GrammarBuilder::addRule(NTSymbol *nt, Sequence *seq) {
  checkForNullptr(nt,  "invalid nullptr for nonterminal");
  checkForNullptr(seq, "invalid nullptr for sequence");
  insertIntoVNt(nt);
  auto ir = rules[nt].insert(seq); // ir: insertion result
  if (!ir.second) { // seq is a duplicate
    delete seq;
    return false; // seq not inserted, so deleted
  } else { // grammar takes ownership of seq
    for (Symbol *sy: *seq) {
      if (sy->isT())
        insertIntoVT(dynamic_cast<TSymbol *>(sy));
      else // sy->isNT()
        insertIntoVNt(dynamic_cast<NTSymbol *>(sy));
    } // for
    return true; // seq inserted
  } // else
} // GrammarBuilder::addRule


void GrammarBuilder::addRule(NTSymbol *nt, initializer_list<Sequence *> seqs) {
  for (auto seq: seqs) {
    checkForNullptr(seq, "invalid nullptr for sequence");
    addRule(nt, seq);
  } // for
} // GrammarBuilder::addRule


void  GrammarBuilder::setNewRoot(NTSymbol *newRoot) {
  checkForNullptr(newRoot, "invalid nullptr for new root nonterminal");
  root = newRoot;
} //  GrammarBuilder::setNewRoot


Grammar *GrammarBuilder::buildGrammar()  const {
  // 1. check if root nonterminal has a rule
  if (rules.find(root) == rules.end())
    throw invalid_argument("root nonterminal \"" +
                           root->name + "\" has no rule");
  // 2. check if all other nonterminals also have rules
  for (auto &rule: rules) {
    for (Sequence *seq: rule.second) {
      for (Symbol *sy: *seq) {
        if (sy->isNT() && sy != root) {
          NTSymbol *nt = dynamic_cast<NTSymbol *>(sy);
          if (rules.find(nt) == rules.end())
            throw invalid_argument("nonterminal \"" +
                                   nt->name + "\" has no rule");
        } // if
      } // for
    } // for
  } // for
  // 3. build the Grammar object
  return new Grammar(root, rules, vNt, vT, v);
} // GrammarBuilder::buildGrammar


ostream &operator<<(ostream &os, const GrammarBuilder &gb) {
  os << "root  = " << *gb.root << endl;
  os << "rules = " << endl;
  for (const auto &p: gb.rules)
    os << "  " << *p.first << " -> " << p.second;
  os << "vNt   = " << gb.vNt << endl;
  os << "vT    = " << gb.vT  << endl;
  return os;
} // operator<<


// === test ============================================================

#if 0

#include "SymbolStuff.cpp"
#include "SequenceStuff.cpp"
#include "GrammarBasics.cpp"
#include "Grammar.cpp"

#ifdef TEST
#error previously included cpp file already defines a main function for testing
#endif
#define TEST

int main(int argc, char *argv[]) {
try {

  cout << "START: GrammarBuilder test" << endl;
  cout << endl;

  SymbolPool *sp = new SymbolPool();
  cout << *sp << endl;


// *** build case selection ***
#define BUILD_CASE 2
// ****************************

  cout << "BUILD_CASE = " << BUILD_CASE << endl << endl;


#if BUILD_CASE == 1

  // 1. build simple grammar programmatically:
  // G(S):
  // S -> EPS | A
  // A -> a | A a

  NTSymbol *S = sp->ntSymbol("S");
  NTSymbol *A = sp->ntSymbol("A");
  TSymbol  *a = sp-> tSymbol("a");

  cout << "NTSymbol S = " << *S << endl;
  cout << "NTSymbol A = " << *A << endl;
  cout << " TSymbol a = " << *a << endl;
  cout << endl;

  Sequence *seq0 = new Sequence(); // empty sequence, epsilon
  Sequence *seq1 = new Sequence(A);
  Sequence *seq2 = new Sequence(a);
  Sequence *seq3 = new Sequence({a, A});

  cout << "Sequence seq0 = " << *seq0 << endl;
  cout << "Sequence seq1 = " << *seq1 << endl;
  cout << "Sequence seq2 = " << *seq2 << endl;
  cout << "Sequence seq3 = " << *seq3 << endl;
  cout << endl;

  GrammarBuilder gb1 = GrammarBuilder(S);
  cout << "empty  GrammarBuilder gb1 (no rules yet):" << endl << gb1 << endl;

  gb1.addRule(S, seq0);
  gb1.addRule(S, seq1);
  gb1.addRule(A, {seq2, seq3});

  cout << "filled GrammarBuilder gb1 (with rules)  :" << endl << gb1 << endl;

#elif BUILD_CASE == 2

  // 2. build grammar from text file:
  GrammarBuilder gb2(string("Grammar1.txt"));
  cout << "filled GrammarBuilder gb2:" << endl << gb2 << endl;

#elif BUILD_CASE == 3

  // 3. build grammar from string literal:
  GrammarBuilder gb3(
    "G(S):             \n\
     S -> A ;          \n\
     A -> a B | B B b  \n\
     B -> b | a b" );
  cout << "filled GrammarBuilder gb3:" << endl << gb3 << endl;

#else // none of the BUILD_TESTCASEs above
  cerr << "ERROR: invalid BUILD_CASE == " << BUILD_CASE << endl;
#endif

  cout << *sp << endl;
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


// end of GrammarBuilder.cpp
//======================================================================
