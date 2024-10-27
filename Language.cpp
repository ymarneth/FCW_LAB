#include "Language.h"

#include <queue>
#include <set>

// Constructor
Language::Language() = default;

// Destructor
Language::~Language() = default;

// Get the sequences
const std::vector<Sequence *> &Language::getSequences() const {
    return sequences;
}

// Recursive function to process non-terminal symbols
void processNonTerminal(const NTSymbol *symbol, const Grammar *g, const Sequence &currentSequence,
                        std::set<Sequence> &allSequences, const int maxLen) {
    std::cout << "Processing non-terminal: " << *symbol << std::endl;

    // If the current sequence has reached the maximum length, stop processing
    if (currentSequence.length() >= maxLen) {
        std::cout << "Reached max length with sequence: " << currentSequence << std::endl;
        return;
    }

    // Get the set of possible expansions for the current non-terminal
    const auto currentSequenceSet = g->rules.find(const_cast<NTSymbol *>(symbol))->second;
    std::cout << "SequenceSet for symbol: " << *symbol << ": " << currentSequenceSet << std::endl;

    for (const Sequence *seq: currentSequenceSet) {
        std::cout << "Looking at sequence: " << *seq << std::endl;

        Sequence newSequence(currentSequence); // Start with a copy of the current sequence
        std::cout << "Current Sequence before expansion: " << newSequence << std::endl;

        bool allTerminals = true; // Flag to ensure all symbols are terminals

        for (const Symbol *symbol1: *seq) {
            if (symbol1->isT()) {
                std::cout << "Terminal symbol found: " << *symbol1 << std::endl;
                newSequence.append(const_cast<Symbol *>(symbol1)); // Append terminal symbol directly
            } else {
                allTerminals = false;
                std::cout << "Non-terminal symbol found: " << *symbol1 << " - Recursing" << std::endl;
                // Recursively process non-terminals in the sequence
                processNonTerminal(dynamic_cast<const NTSymbol *>(symbol1), g, newSequence, allSequences, maxLen);
            }
        }

        // After processing the entire sequence, add only if all symbols are terminals
        if (allTerminals && newSequence.length() <= maxLen) {
            std::cout << "Adding valid terminal-only sequence: " << newSequence << std::endl;
            allSequences.insert(newSequence);
        } else {
            std::cout << "Discarded incomplete or over-length sequence: " << newSequence << std::endl;
        }
    }
}

// Main function to generate the language of a given grammar up to max length
Language *Language::languageOf(const Grammar *g, const int maxLen) {
    auto *language = new Language();

    const auto *root = g->root;
    const Sequence initialSequence; // Start with an empty sequence
    std::set<Sequence> allSequences; // Set to hold unique terminal sequences

    processNonTerminal(root, g, initialSequence, allSequences, maxLen); // Generate sequences

    // Copy valid sequences into the language object
    for (const auto &seq: allSequences) {
        language->sequences.push_back(new Sequence(seq)); // Add each valid sequence to the language
    }

    return language;
}

bool Language::hasSentence(const Sequence *s) const {
    for (const Sequence *seq: sequences) {
        if (*seq == *s) {
            return true;
        }
    }
    return false;
}
