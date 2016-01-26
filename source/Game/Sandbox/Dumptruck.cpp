#include <cmath>

#include "GameVariables.h"
#include "Dumptruck.h"

#define DUMP_SPEED 100
#define DUMP_DURATION 2000

void Dumptruck::Stop()
{
    DEBUG_TRACER(Dumptruck::Stop);

    StateVariable currentDumptruckCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentDumptruckCount.intValue--;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentDumptruckCount);

    StateVariable correctDumptruckCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);
    if(IsValidWord())
    {
        correctDumptruckCount.intValue--;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctDumptruckCount);
    }

    Sprite::Stop();

    if(prefixSprite != null)
    {
        prefixSprite->Stop();
        prefixSprite = null;
    }
}

int dumptruckCount = 0;
bool topTruck = false;

Dumptruck::Dumptruck(GameManager *mgr) : PathFollowingSprite(mgr)
{
    DEBUG_TRACER(Dumptruck::Dumptruck);

    StateVariable currentDumptruckCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentDumptruckCount.intValue++;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentDumptruckCount);

    StateVariable correctDumptruckCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);

    bool shouldBeValid = currentDumptruckCount.intValue < 1 ? true : rand() % currentDumptruckCount.intValue >= correctDumptruckCount.intValue;

    do
    {
        prefix = wordBuilder->GetNextPrefix();
    } while(IsValidWord() != shouldBeValid);

    sprintf(debugId, "Dumptruck - %d(%s)", dumptruckCount, prefix.c_str());
    s3eDebugTracePrintf("Starting new dumptruck %s at: %d", debugId, mgr->GetCurrentClockTime());
    if(IsValidWord())
    {
        correctDumptruckCount.intValue++;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctDumptruckCount);
    }

    SetImage(mgr->GetResourceManager()->GetImage(IMAGE_DUMPTRUCK_FULL));
    AddTransformation(new AnchorTransformation());

    if(topTruck)
    {
        AddSegment(new PathSegmentBezier(CIwVec2(960, 400), CIwVec2(830, 370), CIwVec2(760, 280), CIwVec2(740, 220)));
        AddSegment(new PathSegmentBezier(CIwVec2(740, 220), CIwVec2(720, 190), CIwVec2(740, 140), CIwVec2(485, 140)));
        AddSegment(new PathSegmentBezier(CIwVec2(485, 140), CIwVec2(220, 140), CIwVec2(260, 140), CIwVec2(190, 180)));
        AddSegment(new PathSegmentBezier(CIwVec2(190, 180), CIwVec2(145, 200), CIwVec2(80, 200), CIwVec2(0, 150)));
    }
    else
    {
        AddSegment(new PathSegmentBezier(CIwVec2(0, 150), CIwVec2(270, 320), CIwVec2(190, 400), CIwVec2(440, 400)));
        AddSegment(new PathSegmentBezier(CIwVec2(440, 400), CIwVec2(710, 400), CIwVec2(630, 400), CIwVec2(710, 370)));
        AddSegment(new PathSegmentBezier(CIwVec2(710, 370), CIwVec2(790, 360), CIwVec2(710, 350), CIwVec2(960, 400)));
    }

    topTruck = !topTruck;

    SetTotalPathTimeMillis(10000);
    SetLoopCount(1);

    //SetRenderPath(true);
    //CIwColour renderColor;
    //renderColor.Set(255, 255, 0, 255);
    //SetRenderColor(renderColor);

    mgr->RegisterGameObject(this);

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

    sprintf(prefixSprite->debugId, "Prefix - %d(%s)", dumptruckCount, prefix.c_str());

    dumptruckCount++;
}

bool Dumptruck::IsValidWord()
{
    StateVariable currentSuffix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);

    string checkWord = prefix;
    checkWord += currentSuffix.stringValue;

    return wordBuilder->IsValidWord(checkWord.c_str());
}

void Dumptruck::BeginDump(EventCallback *eventData)
{
    StateVariable points = mgr->GetStateManager()->GetVar(VAR_POINTS);
    if(IsValidWord())
    {
        points.intValue++;
    }
    else
    {
        points.intValue -= 2;
        if(points.intValue < 0)
        {
            points.intValue = 0;
        }
    }
    mgr->GetStateManager()->SetVar(VAR_POINTS, points);
}

void Dumptruck::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Dumptruck::TouchStart);

    CIwFVec2 curLocation((float)GetBaseLocation().x, (float)GetBaseLocation().y);
    CIwFVec2 dumpLocation(470, 330);

    float orientation = IW_ANGLE_TO_RADIANS(GetOrientation(0));

    CIwFVec2 normal;
    normal.x = cos(orientation);
    normal.y = sin(orientation);

    CIwFVec2 cp1 = curLocation + normal * 120;
    CIwFVec2 cp2 =  (dumpLocation - curLocation) / 2;
    cp2 += curLocation + cp2 + normal * cp2.GetLength();

    // Reset the sprite's age so that it will follow the new path starting at the beginning
    birthMillis = mgr->GetCurrentClockTime();

    if(IsValidWord())
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_DING);
    }
    else
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_BUZZ);
    }

    SetPathSpeedPixelsPerSecond(DUMP_SPEED);
    ClearSegments();
    AddSegment(new PathSegmentBezier(Convert(curLocation), Convert(cp1), Convert(cp2), Convert(dumpLocation)));
    SetCompletionCallback(this, (EventCallbackPtr)&Dumptruck::BeginDump);
}