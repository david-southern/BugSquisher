#pragma once

#include <string>
#include <vector>

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

#define MAX_STAR_COUNT 5
#define FULL_STAR_INDEX 5

#define PLACING_STARS 1
#define PLACING_CORRECT_WORDS 2
#define PLACING_MISSED_WORDS 3
#define PLACING_INCORRECT_WORDS 4

class LevelSummary : public EventTarget
{
    GameManager *mgr;

    vector<string> *correctWords, *missedWords, *incorrectWords;
    void Say(char *test, int x, int y);
    void PlaceStar(int starHandle, int starPos);
    void NextAction(EventCallback *eventData);
    Sprite *starSprites[MAX_STAR_COUNT];
    int starHandles[FULL_STAR_INDEX + 1];
    int totalStarsAwarded;

    int animationAction;
    int starsPlaced;
    int correctWordsPlaced;
    int missedWordsPlaced;
    int incorrectWordsPlaced;
    int totalWordsOffered;

public:
    LevelSummary(GameManager *_mgr);
    ~LevelSummary();

    void Reset();
    void Display();    

    void AddCorrectWord(string word);
    void AddIncorrectWord(string word);
    void AddMissedWord(string word);

    int GetTotalStarsAwarded();
};
