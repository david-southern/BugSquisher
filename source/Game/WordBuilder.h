#pragma once

#include <vector>
#include <map>
#include <string>

using namespace std;

class WordBuilder
{
    uint32 prefixIndex;
    uint32 suffixIndex;

    vector<const char *> *shuffledPrefixes;
    vector<const char *> *shuffledSuffixes;
    map<string, bool> *validWords;

public:
    WordBuilder();
    ~WordBuilder();

    const char *GetNextPrefix();
    const char *GetNextSuffix();

    void ShufflePrefixes();
    void ShuffleSuffixes();

    bool IsValidWord(const char *checkWord);
};