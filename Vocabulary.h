// Vocabulary.h:                                          HDO, 2004-2016
// ------------
// Generic class Vocabulary shall be instantiated with [T|NT]Symbol only.
// An object of an instance of the generic class Vocabulary is a set of
// pointers to (possibly different types of) symbols: V = VT + VNT:
// Vocabulary.cpp would not be necessary, but the existing one
// contains a simple test program.
// =====================================================================

#ifndef Vocabulary_h
#define Vocabulary_h

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "ObjectCounter.h"
#include "SymbolStuff.h"


template <typename SyT> // where SyT is either TSymbol or NTSymbol
class Vocabulary: public std::set<SyT *, LessForSymbolPtrs>
        /*OC+*/ , private ObjectCounter<Vocabulary<SyT>> /*+OC*/ {

  private:

    typedef std::set<SyT *, LessForSymbolPtrs> Base;

    // helper class to support binary search for T- and NTSymbols
    class XSymbol final: public SyT
               /*OC+*/ , private ObjectCounter<XSymbol> /*+OC*/ {

      public:

        XSymbol(const std::string &name)
        : SyT(name) {
          // nothing left to do
        } // XSymbol

    }; // XSymbol

  public:

    Vocabulary() = default;
    Vocabulary(const Vocabulary &v) = default;

    Vocabulary &operator=(const Vocabulary &v) = default;

    bool contains(SyT *sy) const {
      return Base::find(sy) != Base::end();
    } // contains

    SyT *symbolFor(const std::string &name) const {
      XSymbol xSy(name); // temporary for search only
      auto it =  Base::find(&xSy);
      if ( it != Base::end() )
        return *it;
      else
        return nullptr;
    } // symbolFor

    bool hasSymbolWith(const std::string &name) const {
      XSymbol xSy(name); // temporary for search only
      return Base::find(&xSy) != Base::end();
    } // hasSymbolWith

}; // Vocabulary<SyT>

template <typename SyT>
static std::ostream &operator<<(std::ostream &os,
                                const Vocabulary<SyT> &v) {
  os << "{ ";
  bool first = true;
  for (auto e: v) {
    if (!first)
      os << ", ";
    os << *e;
    first = false;
  } // fore
  os << " }";
  return os;
} // operator<<


#endif

// end of Vocabulary.h
// =====================================================================
