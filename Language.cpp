#include "Language.h"

#include <algorithm>
#include <queue>
#include <set>

Language::Language() = default;

Language::~Language() = default;

const std::vector<Sequence *> &Language::getSequences() const {
    return sequences;
}

void processNonTerminalSymbols(const NTSymbol *rootSymbol, const Grammar *g, std::set<Sequence> &allSequences,
                               const int maxLen) {
    std::queue<std::pair<Sequence, std::vector<const Symbol *> > > queue;
    queue.push({Sequence(), {rootSymbol}});

    while (!queue.empty()) {
        auto [currentSequence, symbolsToExpand] = queue.front();
        queue.pop();

        // Check if the current sequence contains only terminal symbols
        bool isFullyTerminal = true;
        for (const Symbol *symbol: symbolsToExpand) {
            if (!symbol->isT()) {
                isFullyTerminal = false;
                break;
            }
        }

        // If fully terminal and within maxLen, add it to results and skip further expansion
        if (isFullyTerminal && currentSequence.length() + symbolsToExpand.size() <= maxLen) {
            Sequence fullSequence = currentSequence;
            for (const Symbol *symbol: symbolsToExpand) {
                fullSequence.append(const_cast<Symbol *>(symbol));
            }
            allSequences.insert(fullSequence);
            continue;
        }

        // Stop expanding if the length constraint is reached
        if (currentSequence.length() >= maxLen) {
            continue;
        }

        // Expand the next symbol in `symbolsToExpand` list if not empty
        if (!symbolsToExpand.empty()) {
            const Symbol *nextSymbol = symbolsToExpand.front();
            std::vector<const Symbol *> remainingSymbols(symbolsToExpand.begin() + 1, symbolsToExpand.end());

            if (nextSymbol->isT()) {
                // Append terminal symbol and continue expanding remaining symbols
                Sequence newSequence(currentSequence);
                newSequence.append(const_cast<Symbol *>(nextSymbol));
                queue.emplace(newSequence, remainingSymbols);
            } else {
                // Expand non-terminal by exploring its production rules
                const auto &currentSequenceSet = g->rules.find(
                    const_cast<NTSymbol *>(dynamic_cast<const NTSymbol *>(nextSymbol)))->second;

                for (const Sequence *productionSeq: currentSequenceSet) {
                    Sequence newSequence(currentSequence); // Start with current sequence copy
                    std::vector<const Symbol *> newSymbolsToExpand(remainingSymbols);

                    // Add symbols of this production to the front of remaining symbols
                    newSymbolsToExpand.insert(newSymbolsToExpand.begin(), productionSeq->begin(), productionSeq->end());

                    queue.emplace(newSequence, newSymbolsToExpand);
                }
            }
        }
    }
}

// Adjust the languageOf function to use the BFS version of processNonTerminal
Language *Language::languageOf(const Grammar *g, const int maxLen) {
    auto *language = new Language();
    const auto *root = g->root;
    std::set<Sequence> allSequences;

    // Generate sequences using breadth-first expansion
    processNonTerminalSymbols(root, g, allSequences, maxLen);

    // Copy valid sequences into the language object
    for (const auto &seq: allSequences) {
        language->sequences.push_back(new Sequence(seq));
    }

    return language;
}

bool Language::hasSentence(const Sequence *s) const {
    return std::any_of(
        sequences.begin(),
        sequences.end(),
        [s](const Sequence *seq) { return *seq == *s; }
    );
}

bool Language::hasAllSentences(const std::vector<Sequence *> &sequencesToCheck) const {
    return std::all_of(
        sequencesToCheck.begin(),
        sequencesToCheck.end(),
        [this](const Sequence *s) { return this->hasSentence(s); });
}
