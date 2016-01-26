#include <algorithm>

#include "GameVariables.h"
#include "LevelSummary.h"

LevelSummary::LevelSummary(GameManager *_mgr) : EventTarget(mgr)
{
    mgr = _mgr;

    correctWords = new vector<string>;
    missedWords = new vector<string>;
    incorrectWords = new vector<string>;

    starHandles[0] = IMAGE_STAR_0;
    starHandles[1] = IMAGE_STAR_1;
    starHandles[2] = IMAGE_STAR_2;
    starHandles[3] = IMAGE_STAR_3;
    starHandles[4] = IMAGE_STAR_4;
    starHandles[5] = IMAGE_STAR_5;

    mgr->GetStateManager()->SubscribeEvent("NextLevelSummaryAction", this, (EventCallbackPtr)&LevelSummary::NextAction);
}

LevelSummary::~LevelSummary()
{
    delete correctWords;
    delete missedWords;
    delete incorrectWords;
}

void LevelSummary::Reset()
{
    correctWords->clear();
    missedWords->clear();
    incorrectWords->clear();

    for(int starIndex = 0; starIndex < MAX_STAR_COUNT; starIndex++)
    {
        starSprites[starIndex] = null;
    }

    totalStarsAwarded = 0;
}

void LevelSummary::Say(char *text, int x, int y)
{
    TextSprite *ts = new TextSprite(mgr, text);
    ts->SetAlign(IW_2D_FONT_ALIGN_LEFT);
    ts->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_24));
    ts->SetColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    ts->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    // ts->AddTransformation(new AnchorTransformation());
    ts->SetBaseLocation(CIwVec2(x, y));
    ts->SetZIndex(10);
    mgr->RegisterGameObject(ts);
}


void LevelSummary::PlaceStar(int starCount, int starPos)
{
    Sprite *star = starSprites[starPos];

    if(star == null)
    {
        int centerX = mgr->GetSurfaceWidth() / 2;
        star = new Sprite(mgr);
        star->AddTransformation(new AnchorTransformation());
        star->SetBaseLocation(CIwVec2(centerX - (2 - starPos) * 150, 170));
        star->SetZIndex(10);
        mgr->RegisterGameObject(star);
        starSprites[starPos] = star;
    }

    star->SetImage(mgr->GetResourceManager()->GetImage(starHandles[starCount]));
}

int nextWordX, nextWordY;
string prevWord;
int repeatCount;
TextSprite *lastWordSprite;
double starsSoFar;

void LevelSummary::NextAction(EventCallback *eventData)
{
    int centerX = mgr->GetSurfaceWidth() / 2;
    int wordBuffer = 12;
    int newlinePosition = centerX + 420;

    if(animationAction == PLACING_STARS)
    {
        PlaceStar(0, starsPlaced++);

        if(starsPlaced >= MAX_STAR_COUNT)
        {
            animationAction = PLACING_CORRECT_WORDS;
            char correctWordsBuffer[300];
            sprintf(correctWordsBuffer, "You got %d words correct", correctWords->size());
            Say(correctWordsBuffer, centerX - 420, 250);
            nextWordX = centerX - 420;
            nextWordY = 290;
            prevWord = "***";
            repeatCount = 0;
        }

        mgr->GetStateManager()->PostFutureEvent("NextLevelSummaryAction", mgr->GetCurrentClockTime() + 300);
        return;
    }

    if(animationAction == PLACING_CORRECT_WORDS)
    {
        if(correctWordsPlaced < (int)correctWords->size())
        {
            if(prevWord != correctWords->at(correctWordsPlaced))
            {
                if(prevWord != "***")
                {
                    nextWordX += lastWordSprite->GetWidth() + wordBuffer;
                }

                TextSprite *ts = new TextSprite(mgr, correctWords->at(correctWordsPlaced).c_str());
                ts->SetAlign(IW_2D_FONT_ALIGN_LEFT);
                ts->SetFont(mgr->GetResourceManager()->GetFont(FONT_TREB_16));
                ts->SetColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
                ts->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
                ts->SetZIndex(10);
                mgr->RegisterGameObject(ts);

                // Add 40px to account for possible multiplier text
                if(nextWordX + ts->GetWidth() + 40 > newlinePosition)
                {
                    nextWordX = centerX - 420;
                    nextWordY += ts->GetHeight() + wordBuffer;
                }

                ts->SetBaseLocation(CIwVec2(nextWordX, nextWordY));

                lastWordSprite = ts;
                prevWord = correctWords->at(correctWordsPlaced);
                repeatCount = 1;
            }
            else
            {
                repeatCount++;
                char newText[300];
                sprintf(newText, "%s*%d", prevWord.c_str(), repeatCount);
                lastWordSprite->SetText(newText);
            }

            mgr->GetResourceManager()->PlayAudio(SOUND_DING);
            correctWordsPlaced++;

            starsSoFar = (double)correctWordsPlaced / (double)totalWordsOffered * MAX_STAR_COUNT;
            starsSoFar += 0.001; // Protect against floating-point errors
            int fullStars = (int)(starsSoFar);

            for(int fullStarCount = 0; fullStarCount < fullStars; fullStarCount++)
            {
                PlaceStar(FULL_STAR_INDEX, fullStarCount);
            }

            if(fullStars < MAX_STAR_COUNT)
            {
                int partialStarIndex = (int)((starsSoFar - fullStars) * FULL_STAR_INDEX);
                PlaceStar(partialStarIndex, fullStars);
            }
        }
        else
        {
            animationAction = PLACING_MISSED_WORDS;
            nextWordX = centerX - 420;
            nextWordY += lastWordSprite->GetHeight() + wordBuffer;
            char missedWordsBuffer[300];
            sprintf(missedWordsBuffer, "There were %d valid words that you missed", missedWords->size());
            Say(missedWordsBuffer, nextWordX, nextWordY);
            nextWordX = centerX - 420;
            nextWordY += 34 + wordBuffer;
            prevWord = "***";
            repeatCount = 0;
        }

        mgr->GetStateManager()->PostFutureEvent("NextLevelSummaryAction", (uint32)s3eTimerGetMs() + 200);
        return;
    }

    if(animationAction == PLACING_MISSED_WORDS)
    {
        if(missedWordsPlaced < (int)missedWords->size())
        {
            if(prevWord != missedWords->at(missedWordsPlaced))
            {
                if(prevWord != "***")
                {
                    nextWordX += lastWordSprite->GetWidth() + wordBuffer;
                }

                TextSprite *ts = new TextSprite(mgr, missedWords->at(missedWordsPlaced).c_str());
                ts->SetAlign(IW_2D_FONT_ALIGN_LEFT);
                ts->SetFont(mgr->GetResourceManager()->GetFont(FONT_TREB_16));
                ts->SetColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
                ts->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
                ts->SetZIndex(10);
                mgr->RegisterGameObject(ts);

                // Add 40px to account for possible multiplier text
                if(nextWordX + ts->GetWidth() + 40 > newlinePosition)
                {
                    nextWordX = centerX - 420;
                    nextWordY += ts->GetHeight() + wordBuffer;
                }

                ts->SetBaseLocation(CIwVec2(nextWordX, nextWordY));

                lastWordSprite = ts;
                prevWord = missedWords->at(missedWordsPlaced);
                repeatCount = 1;
            }
            else
            {
                repeatCount++;
                char newText[300];
                sprintf(newText, "%s*%d", prevWord.c_str(), repeatCount);
                lastWordSprite->SetText(newText);
            }

            missedWordsPlaced++;
        }
        else
        {
            animationAction = PLACING_INCORRECT_WORDS;
            nextWordX = centerX - 420;
            nextWordY += lastWordSprite->GetHeight() + wordBuffer;
            char incorrectWordsBuffer[300];
            sprintf(incorrectWordsBuffer, "You picked %d incorrect words", incorrectWords->size());
            Say(incorrectWordsBuffer, nextWordX, nextWordY);
            nextWordX = centerX - 420;
            nextWordY += 34 + wordBuffer;
            prevWord = "***";
            repeatCount = 0;
        }

        mgr->GetStateManager()->PostFutureEvent("NextLevelSummaryAction", (uint32)s3eTimerGetMs() + 200);
        return;
    }

    if(animationAction == PLACING_INCORRECT_WORDS)
    {
        if(incorrectWordsPlaced < (int)incorrectWords->size())
        {
            if(prevWord != incorrectWords->at(incorrectWordsPlaced))
            {
                if(prevWord != "***")
                {
                    nextWordX += lastWordSprite->GetWidth() + wordBuffer;
                }

                TextSprite *ts = new TextSprite(mgr, incorrectWords->at(incorrectWordsPlaced).c_str());
                ts->SetAlign(IW_2D_FONT_ALIGN_LEFT);
                ts->SetFont(mgr->GetResourceManager()->GetFont(FONT_TREB_16));
                ts->SetColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
                ts->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
                ts->SetZIndex(10);
                mgr->RegisterGameObject(ts);

                // Add 40px to account for possible multiplier text
                if(nextWordX + ts->GetWidth() + 40 > newlinePosition)
                {
                    nextWordX = centerX - 420;
                    nextWordY += ts->GetHeight() + wordBuffer;
                }

                ts->SetBaseLocation(CIwVec2(nextWordX, nextWordY));

                lastWordSprite = ts;
                prevWord = incorrectWords->at(incorrectWordsPlaced);
                repeatCount = 1;
            }
            else
            {
                repeatCount++;
                char newText[300];
                sprintf(newText, "%s*%d", prevWord.c_str(), repeatCount);
                lastWordSprite->SetText(newText);
            }

            mgr->GetResourceManager()->PlayAudio(SOUND_BUZZ);
            incorrectWordsPlaced++;

            starsSoFar -= (double)1 / FULL_STAR_INDEX;
            if(starsSoFar < 0)
            {
                starsSoFar = 0;
            }
            starsSoFar += 0.001; // Protect against floating-point errors
            int fullStars = (int)(starsSoFar);

            for(int fullStarCount = 0; fullStarCount < fullStars; fullStarCount++)
            {
                PlaceStar(FULL_STAR_INDEX, fullStarCount);
            }

            if(fullStars < MAX_STAR_COUNT)
            {
                int partialStarIndex = (int)((starsSoFar - fullStars) * FULL_STAR_INDEX);
                PlaceStar(partialStarIndex, fullStars);
                fullStars++;
            }

            for(;fullStars < MAX_STAR_COUNT; fullStars++)
            {
                PlaceStar(0, fullStars);
            }
        }
        else
        {
            animationAction = -1;
            totalStarsAwarded = (int)starsSoFar;
            nextWordX = centerX - 420;
            nextWordY += lastWordSprite->GetHeight() + wordBuffer;
            char finalMessage[300];
            if(totalStarsAwarded > 0)
            {
                sprintf(finalMessage, "Congratulations, you got %d stars!", totalStarsAwarded);
            }
            else
            {
                strcpy(finalMessage, "Sorry, you didn't get any stars.");
            }
            Say(finalMessage, nextWordX, nextWordY);
        }

        mgr->GetStateManager()->PostFutureEvent("NextLevelSummaryAction", (uint32)s3eTimerGetMs() + 200);
        return;
    }
}

int LevelSummary::GetTotalStarsAwarded()
{
    return totalStarsAwarded;
}

void LevelSummary::Display()
{
    sort (correctWords->begin(), correctWords->end());
    sort (missedWords->begin(), missedWords->end());
    sort (incorrectWords->begin(), incorrectWords->end());

    int centerX = mgr->GetSurfaceWidth() / 2;
    int centerY = mgr->GetSurfaceHeight() / 2;

    TextSprite *ts = new TextSprite(mgr, "");
    ts->SetFont(mgr->GetResourceManager()->GetFont(FONT_TREB_16));
    ts->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    ts->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    ts->SetBorderThickness(CIwVec2(7, 7));
    ts->SetPadding(CIwVec2(centerX - 40, centerY - 60));
    ts->AddTransformation(new AnchorTransformation());
    ts->SetBaseLocation(CIwVec2(centerX, centerY));
    ts->SetZIndex(8);
    mgr->RegisterGameObject(ts);

    animationAction = PLACING_STARS;
    starsPlaced = 0;
    correctWordsPlaced = 0;
    missedWordsPlaced = 0;
    incorrectWordsPlaced = 0;
    starsSoFar = 0;
    totalWordsOffered = correctWords->size() + missedWords->size();

    char totalWordsBuffer[300];
    sprintf(totalWordsBuffer, "There were %d words, for a total of 5 stars", totalWordsOffered);
    Say(totalWordsBuffer, centerX - 420, 60);

    mgr->GetStateManager()->PostFutureEvent("NextLevelSummaryAction", mgr->GetCurrentClockTime() + 500);
}

bool skipDuplicates = false;

void LevelSummary::AddCorrectWord(string word)
{
    if(skipDuplicates)
    {
        for(uint checkIndex = 0; checkIndex < correctWords->size(); checkIndex++)
        {
            if(correctWords->at(checkIndex) == word)
            {
                return;
            }
        }
    }

    correctWords->push_back(word);
}

void LevelSummary::AddIncorrectWord(string word)
{
    if(skipDuplicates)
    {
        for(uint checkIndex = 0; checkIndex < incorrectWords->size(); checkIndex++)
        {
            if(incorrectWords->at(checkIndex) == word)
            {
                return;
            }
        }
    }

    incorrectWords->push_back(word);
}

void LevelSummary::AddMissedWord(string word)
{
    if(skipDuplicates)
    {
        for(uint checkIndex = 0; checkIndex < missedWords->size(); checkIndex++)
        {
            if(missedWords->at(checkIndex) == word)
            {
                return;
            }
        }
    }

    missedWords->push_back(word);
}
