#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <vector>
#include "Grammar.h"

class Language {
public:
    Language();
    Language(Language&&) noexcept;

    static Language languageOf(const Grammar *g, int maxLen);

    bool hasSentence(const Sequence &s) const;

    bool hasAllSentences(const std::vector<Sequence> &sequencesToCheck) const;

    const std::vector<Sequence> &getSequences() const;

    ~Language();

private:
    std::vector<Sequence> sequences;
};

#endif
