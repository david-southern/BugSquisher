#include <cmath>

#include "GameVariables.h"
#include "Apple.h"

#define APPLE_MIN_LIFETIME 8000
#define APPLE_MAX_LIFETIME 15000
#define BAD_APPLE_LIFETIME 7000

#define DROP_DURATION 1000

#define LOCATION_COUNT 18

CIwVec2 basketLocation(785, 500);

CIwVec2 validLocations[LOCATION_COUNT] = 
{
    CIwVec2(556, 110),
    CIwVec2(285, 79),
    CIwVec2(241, 166),
    CIwVec2(153, 212),
    CIwVec2(154, 309),
    CIwVec2(224, 375),
    CIwVec2(320, 315),
    CIwVec2(357, 220),
    CIwVec2(241, 268),
    CIwVec2(613, 199),
    CIwVec2(352, 136),
    CIwVec2(430, 267),
    CIwVec2(554, 366),
    CIwVec2(413, 83),
    CIwVec2(465, 184),
    CIwVec2(524, 263),
    CIwVec2(447, 345),
    CIwVec2(623, 319)
};

bool usedLocations[LOCATION_COUNT];
int nextLocationIndex;

void ShuffleLocations()
{
    // Move all used locations to the beginning of the list
    nextLocationIndex = 0;
    for(int index = 0; index < LOCATION_COUNT; index++)
    {
        if(usedLocations[index])
        {
            CIwVec2 tmpVec = validLocations[index];
            validLocations[index] = validLocations[nextLocationIndex];
            validLocations[nextLocationIndex] = tmpVec;
            usedLocations[nextLocationIndex] = true;
            nextLocationIndex++;
        }
    }

    for(int index = nextLocationIndex; index < LOCATION_COUNT; index++)
    {
        int shuffleIndex = rand() % (LOCATION_COUNT - nextLocationIndex) + nextLocationIndex;
        CIwVec2 tmpVec = validLocations[index];
        validLocations[index] = validLocations[shuffleIndex];
        validLocations[shuffleIndex] = tmpVec;
        usedLocations[index] = false;
    }
}

CIwVec2 Apple::GetNewLocation()
{
    StateVariable currentAppleCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);

    if(currentAppleCount.intValue == 1)
    {
        // Level has been reset
        for(int index = 0; index < LOCATION_COUNT; index++)
        {
            usedLocations[index] = false;
        }
        ShuffleLocations();
    }

    if(nextLocationIndex >= LOCATION_COUNT - 1)
    {
        ShuffleLocations();
    }

    usedLocations[nextLocationIndex] = true;
    return validLocations[nextLocationIndex++];
}

void Apple::ReleaseLocation(CIwVec2 location)
{
    for(int index = 0; index < LOCATION_COUNT; index++)
    {
        if(validLocations[index] == location)
        {
            usedLocations[index] = false;
            return;
        }
    }
}

void Apple::Stop()
{
    s3eDebugTracePrintf("Stopping apple %s", debugId);
    DEBUG_TRACER(Apple::Stop);

    ReleaseLocation(GetBaseLocation());

    StateVariable currentAppleCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentAppleCount.intValue--;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentAppleCount);

    if(IsValidWord())
    {
        if(!isBadApple)
        {
            StateVariable currentSuffix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);
            string missedWord = prefix;
            missedWord += currentSuffix.stringValue;
            levelSummary->AddMissedWord(missedWord);
        }

        StateVariable correctAppleCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);
        correctAppleCount.intValue--;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctAppleCount);
    }

    Sprite::Stop();

    if(prefixSprite != null)
    {
        prefixSprite->Stop();
        prefixSprite = null;
    }

    if(currentAppleCount.intValue < APPLE_COUNT)
    {
        new Apple(mgr);
    }
}

int appleCount = 0;

Apple::Apple(GameManager *mgr) : Sprite(mgr)
{
    DEBUG_TRACER(Apple::Apple);

    isBadApple = false;

    StateVariable currentAppleCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentAppleCount.intValue++;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentAppleCount);

    StateVariable correctAppleCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);

    bool shouldBeValid = rand() % currentAppleCount.intValue >= correctAppleCount.intValue;

    do
    {
        prefix = wordBuilder->GetNextPrefix();
    } while(IsValidWord() != shouldBeValid);

    sprintf(debugId, "Apple - %d(%s)", appleCount, prefix.c_str());
    if(IsValidWord())
    {
        correctAppleCount.intValue++;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctAppleCount);
    }

    SetImage(mgr->GetResourceManager()->GetImage(IMAGE_APPLE));
    SetBaseLocation(GetNewLocation());
    AddTransformation(new AnchorTransformation());
    EasingSettings appleEasing = EasingSettings(2000, EasingFunctionElastic, EasingDirectionOut);
    appleEasing.period = 0.4f;
    AddTransformation(new ScaleTransformation(new FloatSourceTween(0, 1, appleEasing)));
    mgr->RegisterGameObject(this);
    mgr->GetStateManager()->SubscribeEvent(EVENT_APPLE_DROP, this, (EventCallbackPtr)&Apple::AppleDrop);

    uint32 dropTime = mgr->GetCurrentClockTime() + rand() % (APPLE_MAX_LIFETIME - APPLE_MIN_LIFETIME) + APPLE_MIN_LIFETIME;
    mgr->GetStateManager()->PostFutureEvent(EVENT_APPLE_DROP, dropTime, this);

    prefixSprite = new TextSprite(mgr, prefix.c_str());
    prefixSprite->SetParent(this);
    prefixSprite->AddTransformation(new AnchorTransformation());
    prefixSprite->AddTransformation(new RotationTransformation(0, Clockwise));
    prefixSprite->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_18));
    prefixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    prefixSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    prefixSprite->SetBorderThickness(CIwVec2(2, 2));
    prefixSprite->SetPadding(CIwVec2(3, 3));
    prefixSprite->SetZIndex(10);
    mgr->RegisterGameObject(prefixSprite);

    sprintf(prefixSprite->debugId, "Prefix - %d(%s)", appleCount, prefix.c_str());
    appleCount++;
}

void Apple::AppleDrop(EventCallback *eventData)
{
    if(eventData->eventData != this)
    {
        return;
    }

    if(isBadApple)
    {
        return;
    }

    s3eDebugTracePrintf("Object %s caught event AppleDrop", debugId);
    CIwVec2 currentLoc = GetBaseLocation();
    ReleaseLocation(currentLoc);
    CIwVec2 dropLoc(currentLoc.x, mgr->GetSurfaceHeight() - GetHeight() + 15);
    SetBaseLocationSource(new LocationSourceTween(currentLoc, dropLoc, EasingSettings(DROP_DURATION, EasingFunctionBounce, EasingDirectionOut)));
    SetLifespan(GetAge(eventData->gameTime) + DROP_DURATION  * 2);
}

void Apple::AppleCollected(EventCallback *eventData)
{
    Stop();
}

bool Apple::IsValidWord()
{
    StateVariable currentSuffix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);

    string checkWord = prefix;
    checkWord += currentSuffix.stringValue;

    return wordBuilder->IsValidWord(checkWord.c_str());
}

void Apple::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Apple::TouchStart);

    if(isBadApple)
    {
        return;
    }

    isBadApple = true;

    StateVariable points = mgr->GetStateManager()->GetVar(VAR_POINTS);
    if(IsValidWord())
    {
        ReleaseLocation(GetBaseLocation());

        mgr->GetResourceManager()->PlayAudio(SOUND_DING);
        points.intValue++;
        
        StateVariable currentSuffix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);
        string correctWord = prefix;
        correctWord += currentSuffix.stringValue;
        levelSummary->AddCorrectWord(correctWord);

        CIwVec2 cp1(630, 260), cp2(785, 450);
        PathFollowingSprite *pathSprite = new PathFollowingSprite(mgr);
        pathSprite->SetTotalPathTimeMillis(DROP_DURATION);
        pathSprite->SetLoopCount(1);
        pathSprite->AddSegment(new PathSegmentBezier(GetBaseLocation(), cp1, cp2, basketLocation));
        pathSprite->SetCompletionCallback(this, (EventCallbackPtr)&Apple::AppleCollected);
        
        // Make sure this apple doesn't expire before the collection path finishes
        SetLifespan(GetAge(mgr->GetCurrentClockTime()) + DROP_DURATION * 10);
        mgr->RegisterGameObject(pathSprite);

        SetParent(pathSprite);
    }
    else
    {
        prefixSprite->Stop();
        prefixSprite = null;

        StateVariable currentSuffix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);
        string incorrectWord = prefix;
        incorrectWord += currentSuffix.stringValue;
        levelSummary->AddIncorrectWord(incorrectWord);

        SetImage(mgr->GetResourceManager()->GetImage(IMAGE_SPLAT));
        SetLifespan(GetAge(mgr->GetCurrentClockTime()) + BAD_APPLE_LIFETIME);
        SetZIndex(-10);
        sprintf(debugId, "Splat - %s", debugId);

        mgr->GetResourceManager()->PlayAudio(SOUND_BUZZ);
        points.intValue -= 2;
        if(points.intValue < 0)
        {
            points.intValue = 0;
        }
    }
    mgr->GetStateManager()->SetVar(VAR_POINTS, points);
}