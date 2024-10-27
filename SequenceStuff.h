// SequenceStuff.h:                                       HDO, 2004-2020
// ---------------
// Classes Sequence and SequenceSet for sets of Sequence objects.
// Sequence objects represent (possibly empty) sequences of
//   pointers to T- and/or NTSymbol objects.
// SequenceSet objects take ownership of their sequences.
// =====================================================================

#ifndef SequenceStuff_h
#define SequenceStuff_h

#include <initializer_list>
#include <memory>
#include <set>
#include <vector>

#include "ObjectCounter.h"
#include "SymbolStuff.h"


// simple global utiltity function also used in other modules
//   throws invalid_argument(errMsg) when ptr == nullptr
void checkForNullptr(void *ptr, const char *errMsg);


// === class Sequence ==================================================

class Sequence: public std::vector<Symbol *>
      /*OC+*/ , private ObjectCounter<Sequence> /*+OC*/ {

  private:

    typedef std::vector<Symbol *> Base;

    Sequence &operator=(const Sequence &seq) = delete;

    void check(int      idx) const;
    void check(iterator it ) const;

  public:

    typedef Base::iterator iterator;

    Sequence() = default; // constructs an empty Sequence aka epsilon
    Sequence(const Sequence &seq) = default;
    Sequence(Symbol *sy);
    Sequence(std::initializer_list<Symbol *> il);

    Sequence(const std::string &str); // based on existing symbols in SymbolPool,
      // e.g., "a B" => Sequence({sp->symbolFor("a"), sp->symbolFor("B")})

    template<typename ItT>
    Sequence(ItT begin, ItT end);

    virtual ~Sequence() = default;

    int length() const; // nr. of terminal and nonterminal symbols
    int terminalLength() const; // nr. of terminal symbols only

    void append(Symbol   *sy);
    void append(Sequence *seq);

    Symbol *&operator[](int      idx);
    Symbol * symbolAt  (int      idx) const;
    Symbol * symbolAt  (iterator it ) const;

    void removeSymbolAt(int      idx);
    void removeSymbolAt(iterator it);

    void replaceSymbolAt(int      idx, Symbol   *sy);
    void replaceSymbolAt(iterator it , Symbol   *sy);

    void replaceSymbolAt(int      idx, Sequence *seq);
    void replaceSymbolAt(iterator it , Sequence *seq);

    bool hasTerminalsOnly() const;

    bool isEpsilon() const; // <=> length() == 0

}; // Sequence

bool operator< (const Sequence &seq1, const Sequence &seq2); // lexicographically
bool operator==(const Sequence &seq1, const Sequence &seq2);

// compares lexicographically as operator< from above
bool lexLessForSequencePtrs(const Sequence *seq1, const Sequence *seq2);

// compares 1. lengths and 2. lexicographically for equal lengths
bool lenLexLessForSequencePtrs(const Sequence *seq1, const Sequence *seq2);

bool equalForSequencePtrs(const Sequence *seq1, const Sequence *seq2);

std::ostream &operator<<(std::ostream &os, const Sequence &seq);


// === class SequenceSet ===============================================
//           SequenceSets take ownership of their Sequences

typedef bool (*SequencePtrCmp)(const Sequence *seq1, const Sequence *seq2);

class SequenceSet: public  std::set<Sequence *, SequencePtrCmp>
         /*OC+*/ , private ObjectCounter<SequenceSet> /*+OC*/ {

  private:

    typedef std::set<Sequence *, SequencePtrCmp> Base;

    SequenceSet &operator=(const SequenceSet &ss) = delete;

  public:

    // constructors installing lexLessForSequencePtrs for std::set
    SequenceSet();
    SequenceSet(const SequenceSet &ss); // makes a deep copy
    SequenceSet(Sequence *s);
    SequenceSet(std::initializer_list<Sequence *> il);

    // constructor allowing different sorting gorder, e.g lenlexLessForSequencePtrs
    SequenceSet(SequencePtrCmp seqPtrCmp);

    virtual ~SequenceSet(); // also deletes its elements (sequences)

    void insertOrDelete(Sequence *&s); // insert s into SequenceSet or ...
                                       // ... delete s if it's already there

}; // SequenceSet

bool operator==(const SequenceSet &ss1, const SequenceSet &ss2);

std::ostream &operator<<(std::ostream &os, const SequenceSet &ss);


#endif

// end of SequenceStuff.h
//======================================================================
