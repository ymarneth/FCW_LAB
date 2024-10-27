// GrammarBasics.h:                                            HDO, 2020
// ---------------
// Basic types for classes GrammarBuilder and Grammar.
//======================================================================

#ifndef GrammarBasics_h
#define GrammarBasics_h

#include <map>
#include <string>

#include "ObjectCounter.h"
#include "Vocabulary.h"
#include "SequenceStuff.h"


// Vocabulary types for the different element (symbol) types of grammars
typedef Vocabulary<NTSymbol> VNt;
typedef Vocabulary< TSymbol> VT;
typedef Vocabulary<  Symbol> V; // for union of VNt and VT

// Rule takes ownership of its SequenceSet (= alternatives)
typedef std::pair<const NTSymbol *, SequenceSet> Rule;

// === class RulesMap ==================================================

class RulesMap: public std::map<NTSymbol *, SequenceSet, LessForSymbolPtrs>
       /*OC+*/, private ObjectCounter<RulesMap> /*+OC*/ {

  typedef std::map<NTSymbol *, SequenceSet, LessForSymbolPtrs> Base;

  static const SequenceSet constEmptyElement;

public:

  using Base::operator[]; // prevent hiding for non-const objects

  // non-inserting operator[] for const RulesMap objects
  const SequenceSet &operator[](NTSymbol *ntSy) const;
    // returns constEmptyElement when ntSy is not in map

}; // RulesMap


#endif

// end of GrammarBasics.h
//======================================================================