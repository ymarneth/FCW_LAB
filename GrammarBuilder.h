// GrammarBuilder.h:                                            HDO, 2020
// ----------------
// GramkmarBuilder implements the builder pattern in order to build
// grammars, i.e. objects of class Grammar.
//======================================================================

#ifndef GrammarBuilder_h
#define GrammarBuilder_h

#include <initializer_list>
#include <iosfwd>
#include <map>
#include <string>

#include "ObjectCounter.h"
#include "SymbolStuff.h"
#include "Vocabulary.h"
#include "SequenceStuff.h"
#include "GrammarBasics.h"


class Grammar;


// === class GrammarBuilder ============================================

class GrammarBuilder final // no public base class
            /*OC+*/ : private ObjectCounter<GrammarBuilder> /*+OC*/ {

  friend std::ostream &operator<<(std::ostream &os, const GrammarBuilder &gb);

  private:

    SymbolPool sp;

    // data components: same as in class Grammar but all non-const

    NTSymbol *root;     // no ownership: SymbolPool is the owner of all symbols
    RulesMap  rules;    // has at least an empty rule for root
    VNt       vNt;      // all nonterminals for rules, including root
    VT        vT;       // all terminals occuring in rules
    V         v;        // all symbols, union of vNt and vT

    void initialize(NTSymbol *root);    // do first part of constructors work

    void readGrammar(std::istream &is); // init. rest of grammar from stream,
      // syntax as generatd by operator<< with one rule/line and one line/rule:
      //   G(S):
      //   S -> seq1 | seq2 | ...
      //   A -> seq3 | ...
      // where seqs are sequences of terminal- and/or nonterminals

    bool insertIntoVNt(NTSymbol *ntSy); // true if inserted else sy is a duplicate
    bool insertIntoVT (TSymbol  *tSy);  // true if inserted else sy is a duplicate

  public:

    GrammarBuilder(NTSymbol *root); // empty builder, needs programmatical init.

    GrammarBuilder(const std::string &fileName); // init. with contents of text file
    GrammarBuilder(const char        *str);      // init. with C string

    GrammarBuilder(const GrammarBuilder &gb) = delete;
    GrammarBuilder &operator==(const GrammarBuilder &gb) = delete;

    ~GrammarBuilder() = default; // non-virtual as class is final

    // methods for programmatical construction:

    bool addRule(NTSymbol *nt, Sequence *seq); // inserts a Sequence ...
      // ... into the rule for nt, returns
      //   true  if seq was a new one, that has been added
      //   false if seq was a duplicate, so addRule deleted seq

    void addRule(NTSymbol *nt, std::initializer_list<Sequence *> seqs);

    void setNewRoot(NTSymbol *newRoot);

    // main method to build new Grammar object
    Grammar *buildGrammar() const;

}; // GrammarBuilder

std::ostream &operator<<(std::ostream &os, const GrammarBuilder &gb);


#endif

// end of GrammarBuilder.h
//======================================================================