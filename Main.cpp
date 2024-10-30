// Main.cpp:                                             HDO, 2004-2022
// --------
// Main program for the object-oriented representation of formal
// languages using classes for symbols, sequences and grammars.
// ====================================================================

#include <algorithm>
#include <iostream>
#include <typeinfo>

#include "Language.h"
#include "SignalHandling.h"
#include "Timer.h"
#include "SymbolStuff.h"
#include "SequenceStuff.h"
#include "Vocabulary.h"
#include "GrammarBasics.h"
#include "GrammarBuilder.h"
#include "Grammar.h"

using namespace std;

bool containsEpsilonOrMarkedNT(const Sequence &seq, const VNt &epsilonNonterminals) {
    return std::any_of(seq.begin(), seq.end(), [&epsilonNonterminals](Symbol *s) {
        return s->isNT() && epsilonNonterminals.contains(dynamic_cast<NTSymbol *>(s));
    });
}

void addSequenceIfNonDeletable(GrammarBuilder &epsilonFreeBuilder, const Sequence &seq, NTSymbol *nt,
                               const VNt &epsilonNonterminals) {
    const auto seqContainsEpsilonOrMarkedNT = containsEpsilonOrMarkedNT(seq, epsilonNonterminals);
    if (!seqContainsEpsilonOrMarkedNT && !seq.isEpsilon()) {
        epsilonFreeBuilder.addRule(nt, new Sequence(seq));
    }
}

vector<Sequence *> generateEpsilonFreeCombinations(const Sequence &seq, const VNt &epsilonNonterminals) {
    vector<Sequence *> result;
    result.push_back(new Sequence());

    for (Symbol *s: seq) {
        const size_t currentSize = result.size();
        if (s->isNT() && epsilonNonterminals.contains(dynamic_cast<NTSymbol *>(s))) {
            // For epsilon-producing NTs, include both with and without NT
            for (size_t i = 0; i < currentSize; ++i) {
                result.push_back(new Sequence(*result[i])); // Copy existing sequences
                result.back()->append(s); // Append NT instance
            }
        } else {
            // Append current symbol to all sequences in the result set
            for (size_t i = 0; i < currentSize; ++i) {
                result[i]->append(s);
            }
        }
    }

    // Remove fully epsilon sequences
    result.erase(remove_if(result.begin(), result.end(),
                           [](const Sequence *s) { return s->empty() || s->isEpsilon(); }),
                 result.end());

    return result;
}

void explodeDeletableRules(GrammarBuilder &epsilonFreeBuilder, const Sequence &seq, NTSymbol *nt,
                           const VNt &epsilonNonterminals) {
    const auto containsMarkedNT = containsEpsilonOrMarkedNT(seq, epsilonNonterminals);
    if (!containsMarkedNT) return;

    vector<Sequence *> newCombinations = generateEpsilonFreeCombinations(seq, epsilonNonterminals);
    for (Sequence *newSeq: newCombinations) {
        if (!newSeq->isEpsilon()) {
            epsilonFreeBuilder.addRule(nt, newSeq);
        }
    }
}

Grammar *newEpsilonFreeGrammar(const Grammar *g) {
    // Initialize a new grammar builder with the same root
    const auto epsilonFreeBuilder = std::make_unique<GrammarBuilder>(g->root);

    // Step 1 Mark all deletable non-terminals
    const VNt epsilonNonterminals = g->deletableNTs();
    cout << "Deletable non-terminals: " << epsilonNonterminals << endl;

    for (const auto &rule: g->rules) {
        auto const &nt = rule.first;
        auto const &sequenceSet = rule.second;
        for (const Sequence *seq: sequenceSet) {
            // Step 2: Copy all rules without epsilon or marked NTs on the right side
            addSequenceIfNonDeletable(*epsilonFreeBuilder, *seq, nt, epsilonNonterminals);

            // Step 3: Generate all possible combinations for rules with marked NTs
            explodeDeletableRules(*epsilonFreeBuilder, *seq, nt, epsilonNonterminals);
        }
    }

    // Step 4: Add S' -> S | ε if S is deletable
    if (epsilonNonterminals.contains(g->root)) {
        cout << "Root is deletable" << endl;
        SymbolPool sp;
        auto *optS = sp.ntSymbol("S'");
        epsilonFreeBuilder->addRule(optS, new Sequence(g->root));
        epsilonFreeBuilder->addRule(optS, new Sequence());
        epsilonFreeBuilder->setNewRoot(optS);
    }

    Grammar *resultGrammar = epsilonFreeBuilder->buildGrammar();
    return resultGrammar;
}

int main(int argc, char *argv[]) {
    installSignalHandlers();

    cout << "START Main" << endl;
    cout << endl;
    startTimer();

    try {
        auto *sp = new SymbolPool();
        cout << *sp << endl;

        GrammarBuilder *gb1 = nullptr;
        GrammarBuilder *gb2 = nullptr;
        GrammarBuilder *gb3 = nullptr;

        Grammar *g1 = nullptr;
        Grammar *g2 = nullptr;
        Grammar *g3 = nullptr;


        // *** test case selection: 1, 2, or 3 ***
#define TESTCASE 5
        // ***************************************

        cout << "TESTCASE " << TESTCASE << endl << endl;

#if TESTCASE == 1 // programmatical grammar construction

  // G(S):
  // S -> A ;
  // A -> a B | B B b
  // B -> b | a b

  NTSymbol *S  = sp->ntSymbol("S");
  NTSymbol *A  = sp->ntSymbol("A");
  NTSymbol *B  = sp->ntSymbol("B");

   TSymbol *a  = sp-> tSymbol("a");
   TSymbol *b  = sp-> tSymbol("b");
   TSymbol *sc = sp-> tSymbol(";");

  gb1 = new GrammarBuilder(S);

#if 0 // with separate Sequence variables ...

  Sequence *seq1 = new Sequence({A, sc});
  Sequence *seq2 = new Sequence({a, B});
  Sequence *seq3 = new Sequence({B, B, b});
  Sequence *seq4 = new Sequence(b);
  Sequence *seq5 = new Sequence({a, b});

  gb1->addRule(S, seq1);
  gb1->addRule(A, {seq2, seq3});
  gb1->addRule(B, {seq4, seq5});

#else // .. much simpler with Sequences constructed in place
  gb1->addRule(S,  new Sequence({A, sc}));
  gb1->addRule(A, {new Sequence({a, B}),
                   new Sequence({B, B, b})});
  gb1->addRule(B, {new Sequence(b),
                   new Sequence({a, b})});
#endif

  g1 = gb1->buildGrammar();
  cout << "grammar constructed programmatically:" << endl << *g1 << endl;


#elif TESTCASE == 2 // grammar construction from text file

  gb2 = new GrammarBuilder(string("G.txt"));
  g2 = gb2->buildGrammar();
  // or for short: g2 = GrammarBuilder(string("G.txt")).buildGrammar();

  cout << "grammar from text file:" << endl << *g2 << endl;


#elif TESTCASE == 3 // grammar construction from C string literal

  gb3 = new GrammarBuilder(
    "G(S):                          \n\
     S -> E ;                       \n\
     E -> a A b E | b B a E | eps   \n\
     A -> a A b A | eps             \n\
     B -> b B a B | eps             ");
  g3 = gb3->buildGrammar();
  // or for short: g3 = GrammarBuilder("...").buildGrammar();

  cout << "grammar from C string:" << endl << *g3 << endl;

#elif TESTCASE == 4 // grammar construction from text file

        // Test case 4: Test epsilon-free grammar conversion
        const GrammarBuilder gb4(
            "G(S):                     \n\
                 S -> A B C               \n\
                 A -> B B | eps           \n\
                 B -> C C | a             \n\
                 C -> A A | b             ");

        const auto *originalGrammar = gb4.buildGrammar();
        cout << "Original Grammar with epsilon rules:" << endl;
        cout << *originalGrammar << endl;

        const auto *epsilonFreeGrammar = newEpsilonFreeGrammar(originalGrammar);
        cout << endl << "Epsilon-Free Grammar:" << endl;
        cout << *epsilonFreeGrammar << endl;

        // Clean up
        delete originalGrammar;
        delete epsilonFreeGrammar;

#elif TESTCASE == 5

        const GrammarBuilder gb(
            "G(S):                      \n\
    S -> a B | b A                 \n\
    A -> a | a S | b A A           \n\
    B -> b | b S | a B B            ");
        const Grammar *g = gb.buildGrammar();

        cout << "Grammar:" << endl;
        cout << *g << endl;

        constexpr int maxLength = 6;
        const auto language = Language::languageOf(g, maxLength);

        const auto &sequences = language.getSequences();
        std::cout << "\nGenerated language sequences up to length " << maxLength << ":\n";
        for (const auto &seq: sequences) {
            std::cout << seq << std::endl;
        }

        TSymbol *a = sp->tSymbol("a");
        TSymbol *b = sp->tSymbol("b");

        std::vector<Sequence> expectedSequences = {
            {a, a, a, b, b, b}, {a, a, b, a, b, b}, {a, a, b, b}, {a, a, b, b, a, b},
            {a, a, b, b, b, a}, {a, b}, {a, b, a, a, b, b}, {a, b, a, b}, {a, b, a, b, a, b},
            {a, b, a, b, b, a}, {a, b, b, a}, {a, b, b, a, a, b}, {a, b, b, a, b, a},
            {a, b, b, b, a, a}, {b, a}, {b, a, a, a, b, b}, {b, a, a, b}, {b, a, a, b, a, b},
            {b, a, a, b, b, a}, {b, a, b, a}, {b, a, b, a, a, b}, {b, a, b, a, b, a},
            {b, a, b, b, a, a}, {b, b, a, a}, {b, b, a, a, a, b}, {b, b, a, a, b, a},
            {b, b, a, b, a, a}, {b, b, b, a, a, a}
        };

        if (sequences.size() != expectedSequences.size()) {
            throw std::runtime_error("Error: The number of generated sequences does not match the expected count.");
        }

        for (const auto &seq: expectedSequences) {
            if (!language.hasSentence(seq)) {
                throw std::runtime_error("Error: Required sequence missing from language.");
            }
        }

        std::cout << "All required sequences are present in the language." << std::endl;

        delete g;

#else // none of the TESTCASEs above

  cerr << "ERROR: invalid TESTCASE " << TESTCASE << endl;

#endif

        delete gb1;
        delete gb2;
        delete gb3;

        delete g1;
        delete g2;
        delete g3;

        cout << endl << *sp << endl; // final contents of symbol pool
        delete sp;
    } catch (const exception &e) {
        cerr << "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
    } // catch

    stopTimer();
    cout << "elapsed time: " << elapsed() << endl;
    cout << endl;
    cout << "END Main" << endl;

    // cout << "type CR to continue ...";
    // getchar();

    return 0;
} // main


// end of Main.cpp
//======================================================================
