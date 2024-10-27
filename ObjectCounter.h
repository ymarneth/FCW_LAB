// ObjectCounter.h:                                       HDO, 2013-2019
// ---------------
// Simple mechanism to count objects of any user defined class (UDC) and
// to report garbage on program termination.
//
// Simple usage for singular classes:
//   class UDC: ..., private ObjectCounter<UDC> { ... }; // UDC
//
// If objects of a class hierarchy (with single inheritance only)
// have to be counted, template class ObjectCounter<UDC[, BASE]>
// * has to be a private base class for every class in the hierarchy and
// * has to be instantiated with the class and its base class.
// Example for root class A and derived classes B and C:
//   class A: ...,      private ObjectCounter<A>    { ... }; // A
//   class B: public A, private ObjectCounter<B, A> { ... }; // B
//   class C: public B, private ObjectCounter<C, B> { ... }; // C
//
// To (de-)activate object counting for classes, special C comments
// (shown below with \* instead of /*) can be used.
// This is how the activated (+) case looks like:
//   class UDC: public  BASE
//     \*OC+*\ , private ObjectCounter<UDC> \*+OC*\ { ... }; // UDC
// And this is how the deactivated (-) case looks like:
//   class UDC: public  BASE
//     \*OC-   , private ObjectCounter<UDC>   -OC*\ { ... }; // UDC
// Switching can easily be incorporated via find and replace.
//
// Implementation based on the curiously recurring template pattern (CRTP),
// see: en.wikipedia.org/wiki/Curiously_recurring_template_pattern.
//
// Additionally, the idiom construct on first use is applied to guarantee
// a correct initialization order of global objects,
// see: en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Construct_On_First_Use
// =====================================================================

#ifndef ObjectCounter_h
#define ObjectCounter_h

#include <cstdlib>
#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>
#include <utility>


// 1. ACTIVATION: activate object counting
#define DO_OBJECT_COUNTING


#ifndef DO_OBJECT_COUNTING // NO OBJECT COUNTING


  template <class UDC, class BASE = UDC>
  class ObjectCounter {     // dummy object counter, has nothing to do
  }; // ObjectCounter<UDC, BASE>


#else                       // DO OBJECT COUNTING


// 2. LOGGING: additionally activate logging of objects
#define LOG_OBJECTS         // log objects in a map to report garbage ...
#undef LOG_OBJECTS_TO_FILE // ... additionally in file ObjectCounterLog.txt

// 3. GARBAGE: additionally throw an exception on construction of garbage object
#undef EXCEPT_ON_CONSTR_OF_GARBAGE // on define, specify class and nr. of constr.:
#define DEMANGLED_CLASS_NAME "N.N"
#define CONSTR_NUMBER         1


#ifdef LOG_OBJECTS_TO_FILE
static std::ofstream &oclog() {
  static std::unique_ptr<std::ofstream> p(
                     new std::ofstream("ObjectCounterLog.csv"));
  static bool firstCall = true;
  if (firstCall) {
    firstCall = false;
    (*p) << "class name; \t+/-nr; \taddress" << std::endl;
  } // if
  return *p;
} // oclog
#endif

#if (defined(__GNUC__) || defined (__clang__))
#include <cxxabi.h>
static std::string demangled(const std::string &name) {
  int status;
  char *dnp = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
  if (status != 0)
    throw std::runtime_error("abi::__cxa_demangle returned invalid status");
  std::string dmn = dnp;
  std::free((void*)dnp);    // free, as abi::__cxa_demangle used malloc
  return dmn;
} // demangled
#elif (defined(_MSC_VER))   // Microssoft cl
static std::string demangled(const std::string &name) {
  return name;              // nothing to demangle for cl
} // demangled
#else
  #error compiler not supported (name demangling not possible)
#endif

static void throw_runtime_error(const std::string msg, const std::string className) {
#ifdef LOG_OBJECTS_TO_FILE
  oclog().close();
#endif
  throw std::runtime_error(msg + " for class " + className);
} // throw_runtime_error


class OCData final { // data for the ObjectCounters: one OCData object per class

  private:

    // map class names to their OCData objects to correct base class counters
    typedef std::unordered_map<std::string, OCData *> ocdMap;

    static ocdMap &ocdm() {
      static std::unique_ptr<ocdMap> p(new ocdMap);
      return *p;
    } // ocdm

    const std::string className;     // format depends on RTTI, roughly "...UDC..."
    const std::string baseClassName; // format depends on RTTI, roughly "...BASE..."
    const std::string demangledClassName;
    const bool hasBaseClass;         // true <==> className =! baseClassName
    int nConstr, nDestr;             // number of constructions and destructions
#ifdef LOG_OBJECTS
    std::unordered_map<void *, int> om; // object map: address -> nConstr
  #ifdef LOG_OBJECTS_TO_FILE
  #endif
#endif

  public:

    OCData(                  ) = delete;
    OCData(const OCData  &ocd) = delete;
    OCData(      OCData &&ocd) = delete;

    OCData(const std::string &    className,
           const std::string &baseClassName)
    : className(className),
      baseClassName(baseClassName),
      demangledClassName(demangled(className)),
      hasBaseClass(className != baseClassName),
      nConstr(0), nDestr(0)
#ifdef LOG_OBJECTS
      , om()
#endif
    {
      ocdm()[className] = this; // register OCData for className
    } // OCData

    OCData &operator=(const OCData  &ocd) = delete;
    OCData &operator=(      OCData &&ocd) = delete;

    void countConstr(void *otc) { // object to count
      if (hasBaseClass)
        ocdm()[baseClassName]->nConstr--;
      nConstr++;
#ifdef LOG_OBJECTS
  #ifdef LOG_OBJECTS_TO_FILE
      oclog() << demangledClassName << "; \t+" << nConstr << "; \t" << otc << std::endl;
  #endif
  #ifdef EXCEPT_ON_CONSTR_OF_GARBAGE
      if (demangledClassName == DEMANGLED_CLASS_NAME &&
          nConstr            == CONSTR_NUMBER)
        throw_runtime_error("construction of garbage object", demangledClassName);
  #endif
      auto ir = om.insert(std::make_pair(otc, nConstr));
      if (!ir.second)       // otc already has been an element of om
        throw_runtime_error("re-construction of object", demangledClassName);
#endif
    } // countConstr

    void countDestr(void *otc) {
      if (hasBaseClass)
        ocdm()[baseClassName]->nDestr--;
      nDestr++;
#ifdef LOG_OBJECTS
  #ifdef LOG_OBJECTS_TO_FILE
      oclog() << demangledClassName << "; \t-" << nDestr << "; \t" << otc << std::endl;
  #endif
      auto ec = om.erase(otc);
      if (ec == 0)          // otc has not been an element of om
        throw_runtime_error("destruction of unknown object", demangledClassName);
#endif
    } //countDestr

    ~OCData() {  // non virtual as class is final
      static bool firstCallToDestr = true;
      int nAlive = nConstr - nDestr;
      if (!std::cout.good()) // sorry, std::cout is not available any more
        return;
      if (firstCallToDestr) {
        firstCallToDestr = false;
        std::cout << std::endl << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "report generated on destruction of ObjectCounter<>s:" << std::endl;
#ifdef LOG_OBJECTS_TO_FILE
        std::cout << "(details in file ObjectCounterLog.csv)" << std::endl;
#endif
      } // if
      std::cout << std::endl;
      std::cout << demangledClassName << ": " << std::endl <<
                   "  +" << nConstr << " -" << nDestr << " = " << nAlive << " alive";
      if (nAlive == 0)
         std::cout << std::endl;
      else { // nAlive > 0
         std::cout << " -> GARBAGE!" << std::endl;
#ifdef LOG_OBJECTS
        std::unordered_map<int, void *> iom; // inverted om: nConstr -> address
        for (const auto &e: om)
          iom[e.second] = e.first;
        int i = 1;
        for (const auto &e: iom)
          std::cout << "  " << i++ << ". "
                    << "constrNr = "   << e.first
                    << ", address = "  << e.second << std::endl;
#endif
      } // else
    } // ~OCData

}; // OCData


template <class UDC, class BASE = UDC> // UDC is a user defined class ...
class ObjectCounter { //   ... privately derived from ObjectCounter ...
  // ... to count objects of class UDC using an OCData object (ocd)

  private:

    static OCData &ocd() {
      static std::unique_ptr<OCData> p(
                         new OCData(std::string(typeid(UDC ).name()),
                                    std::string(typeid(BASE).name())));
      return *p;
    } // ocd

  protected:

    ObjectCounter() {
      ocd().countConstr(this);
    } // ObjectCounter

    ObjectCounter(const ObjectCounter & /*oc*/) {
      ocd().countConstr(this);
    } // ObjectCounter

    ObjectCounter(      ObjectCounter &&/*oc*/) {
      ocd().countConstr(this);
    } // ObjectCounter

    ObjectCounter &operator=(const ObjectCounter  &/*oc*/) = default;
    ObjectCounter &operator=(      ObjectCounter &&/*oc*/) = default;

    virtual ~ObjectCounter() {
      ocd().countDestr(this);
    } // ~ObjectCounter

}; // ObjectCounter<UDC>


#endif // DO_OBJECT_COUNTING


#endif

// end of ObjectCounter.h
// =====================================================================
