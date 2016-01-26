#include <cmath>

#include "GameVariables.h"
#include "Bug.h"

#define MIN_PATHS_PER_BUG 3
#define CONTROL_POINT_BUFFER 20
#define END_POINT_BUFFER 100

#define PREFIX_RIGHT 200
#define PREFIX_BOTTOM 200
// #define SCOREBOX_TOP 500
#define SCOREBOX_TOP 640
#define POINTS_LEFT 700
#define POINTS_BOTTOM 75

#define CENTER_TARGET_SIZE 400

#define LADYBUG_FRAMES 1
#define LADYBUG_FPS 1

CIwFVec2 Bug::GetRandomPoint(int buffer)
{
    int maxWidth = mgr->GetSurfaceWidth() - buffer * 2;
    int maxHeight = mgr->GetSurfaceHeight() - buffer * 2;
    int x, y;
    bool validPoint = false;

    while(!validPoint)
    {
        validPoint = true;

        x = rand() % maxWidth + buffer;
        y = rand() % maxHeight + buffer;

        // Avoid the prefix area
        if(x < PREFIX_RIGHT + buffer && y < PREFIX_BOTTOM + buffer)
        {
            validPoint = false;
        }

        // Avoid the score boxes
        if(y > SCOREBOX_TOP - buffer)
        {
            validPoint = false;
        }

        // Avoid the points label
        if(x > POINTS_LEFT - buffer && y < POINTS_BOTTOM + buffer)
        {
            validPoint = false;
        }
    }

    return CIwFVec2((float)x, (float)y);
}

void Bug::Stop()
{
    DEBUG_TRACER(Bug::Stop);

    StateVariable currentBugCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentBugCount.intValue--;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentBugCount);

    if(IsValidWord())
    {
        StateVariable correctBugCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);
        correctBugCount.intValue--;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctBugCount);
    }

    FlipbookSprite::Stop();
    pathSprite->Kill(); // Don't run the path's completion event if we stopped it ourselves
    suffixSprite->Stop();

    if(currentBugCount.intValue < BUG_COUNT)
    {
        // Patch aroung a defect where we occasionally get more than BUG_COUNT bugs on the screen
        new Bug(mgr);
    }
}

float randFactor;

CIwFVec2 Bug::ConstrainToScreen(CIwFVec2 startPoint, CIwFVec2 normal, float padding)
{
    float maxLen, checkLen;

    if(normal.y < 0)
    {
        maxLen = (startPoint.y - padding) / normal.y;
    }
    else
    {
        maxLen = (mgr->GetSurfaceHeight() - padding - startPoint.y) / normal.y;
    }

    if(normal.x < 0)
    {
        checkLen = (startPoint.x - padding) / normal.x;
    }
    else
    {
        checkLen = (mgr->GetSurfaceWidth() - padding - startPoint.x) / normal.x;
    }

    maxLen = fabs(maxLen);
    checkLen = fabs(checkLen);

    if(checkLen < maxLen)
    {
        maxLen = checkLen;
    }

    float actualLen = randFactor * maxLen;

    normal *= actualLen;

    return startPoint + normal;
}

void Bug::GetNextPath(bool lastPath)
{
    DEBUG_TRACER(Bug::GetNextPath);
    startPoint = endPoint;

    // Make sure that the new CP1 is on the same line as that from the old CP2 to the new startPoint.  This
    // will assure that the bug's orientation will not change radically when transitioning from one curve
    // to the next

    CIwFVec2 normal(startPoint - CP2);
    normal.Normalise();
    randFactor = ((rand() % 500) + 500) / 1000.0f;
    CP1 = ConstrainToScreen(startPoint, normal, 25);

    CP2 = GetRandomPoint(CONTROL_POINT_BUFFER);
    if(lastPath)
    {
        int entrySide = rand() % 4;

        // Make sure that the ending point is off screen.  Don't enter/exit over any of the UI elements
        switch(entrySide)
        {
        case 0: // Exit stage left
            endPoint.x = (float)(-(int)GetWidth());
            endPoint.y = (float)(rand() % (SCOREBOX_TOP - PREFIX_BOTTOM - END_POINT_BUFFER * 2) + PREFIX_BOTTOM + END_POINT_BUFFER);
            break;

        case 1: // Exit stage right
            endPoint.x = (float)(mgr->GetSurfaceWidth() + GetWidth());
            endPoint.y = (float)(rand() % (SCOREBOX_TOP - POINTS_BOTTOM - END_POINT_BUFFER * 2) + POINTS_BOTTOM + END_POINT_BUFFER);
            break;

        case 2: // Exit stage top
            endPoint.y = (float)(-(int)GetHeight());
            endPoint.x = (float)(rand() % (POINTS_LEFT - PREFIX_RIGHT - END_POINT_BUFFER * 2) + PREFIX_RIGHT + END_POINT_BUFFER);
            break;

        case 3: // Exit stage bottom
            endPoint.y = (float)(mgr->GetSurfaceHeight() + GetHeight());
            endPoint.x = (float)(rand() % mgr->GetSurfaceWidth());
            break;

        }
    }
    else
    {
        endPoint = GetRandomPoint(END_POINT_BUFFER);
    }

    CIwVec2 sp; 
    sp.x = (int)startPoint.x;
    sp.y = (int)startPoint.y;

    CIwVec2 cp1; 
    cp1.x = (int)CP1.x;
    cp1.y = (int)CP1.y;

    CIwVec2 cp2; 
    cp2.x = (int)CP2.x;
    cp2.y = (int)CP2.y;

    CIwVec2 ep; 
    ep.x = (int)endPoint.x;
    ep.y = (int)endPoint.y;

    pathSprite->AddSegment(new PathSegmentBezier(sp, cp1, cp2, ep));
}

int bugCount = 0;

Bug::Bug(GameManager *mgr) : FlipbookSprite(mgr)
{
    DEBUG_TRACER(Bug::Bug);

    StateVariable currentBugCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_THINGY_COUNT);
    currentBugCount.intValue++;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, currentBugCount);

    StateVariable correctBugCount = mgr->GetStateManager()->GetVar(VAR_CURRENT_CORRECT_THINGY_COUNT);

    bool shouldBeValid = rand() % currentBugCount.intValue >= correctBugCount.intValue;

    do
    {
        suffix = wordBuilder->GetNextSuffix();
    } while(IsValidWord() != shouldBeValid);

    if(IsValidWord())
    {
        correctBugCount.intValue++;
        mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, correctBugCount);
    }

    pathSprite = new PathFollowingSprite(mgr);
    pathSprite->SetPathSpeedPixelsPerSecond(BUG_SPEED);
    pathSprite->SetLoopCount(1);
    pathSprite->SetCompletionCallback(this, (EventCallbackPtr)&Bug::BugFinished);
    mgr->RegisterGameObject(pathSprite);

    this->SetParent(pathSprite);
    CIw2DImage *ladybug = mgr->GetResourceManager()->GetImage(IMAGE_LADYBUG);
    this->SetAnimationParams(ladybug, ladybug->GetWidth() / LADYBUG_FRAMES, ladybug->GetHeight(), LADYBUG_FRAMES, LADYBUG_FPS);
    this->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(this);

    suffixSprite = new TextSprite(mgr, suffix.c_str());
    suffixSprite->SetParent(pathSprite);
    suffixSprite->AddTransformation(new AnchorTransformation());
    suffixSprite->AddTransformation(new RotationTransformation(0, Clockwise));
    suffixSprite->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_18));
    suffixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    suffixSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    suffixSprite->SetBorderThickness(CIwVec2(2, 2));
    suffixSprite->SetPadding(CIwVec2(3, 3));
    suffixSprite->SetZIndex(10);
    mgr->RegisterGameObject(suffixSprite);

    sprintf(this->debugId, "Bug - %d", bugCount);
    sprintf(suffixSprite->debugId, "Suffix - %d", bugCount);
    sprintf(pathSprite->debugId, "Path - %d", bugCount);
    bugCount++;

#ifdef DO_DEBUG_TRACING
    s3eDebugTracePrintf("Creating bug %s", debugId);
#endif
    // Initialize endPoint here, because GetNextPath() begins by assigning endPoint to startPoint
    // Make sure that the starting point is off screen.  Also make sure that CP2 is 'behind' the starting
    // point.  GetNextPath uses CP2 to calculate the initial tangent of the path, so we want the path 
    // to be moving onto the screen
    int entrySide = rand() % 4;

    // Make sure that the ending point is off screen.  Don't enter/exit over any of the UI elements
    switch(entrySide)
    {
    case 0: // Exit stage left
        endPoint.x = (float)(-(int)GetWidth());
        endPoint.y = (float)(rand() % (SCOREBOX_TOP - PREFIX_BOTTOM - END_POINT_BUFFER * 2) + PREFIX_BOTTOM + END_POINT_BUFFER);
        break;

    case 1: // Exit stage right
        endPoint.x = (float)(mgr->GetSurfaceWidth() + GetWidth());
        endPoint.y = (float)(rand() % (SCOREBOX_TOP - POINTS_BOTTOM - END_POINT_BUFFER * 2) + POINTS_BOTTOM + END_POINT_BUFFER);
        break;

    case 2: // Exit stage top
        endPoint.y = (float)(-(int)GetHeight());
        endPoint.x = (float)(rand() % (POINTS_LEFT - PREFIX_RIGHT - END_POINT_BUFFER * 2) + PREFIX_RIGHT + END_POINT_BUFFER);
        break;

    case 3: // Exit stage bottom
        endPoint.y = (float)(mgr->GetSurfaceHeight() + GetHeight());
        endPoint.x = (float)(rand() % mgr->GetSurfaceWidth());
        break;

    }

    CIwFVec2 centerPoint((float)mgr->GetSurfaceWidth() / 2, (float)mgr->GetSurfaceHeight() / 2);
    CIwFVec2 centerOffset((float)(rand() % CENTER_TARGET_SIZE - CENTER_TARGET_SIZE / 2), (float)(rand() % CENTER_TARGET_SIZE - CENTER_TARGET_SIZE / 2));
    centerPoint = centerPoint + centerOffset;
    CP2 = endPoint + (endPoint - centerPoint);

    int numPaths = MIN_PATHS_PER_BUG;

    for(int pathIndex = 0; pathIndex < numPaths; pathIndex++)
    {
        GetNextPath(pathIndex == numPaths - 1);
    }    
}

void Bug::BugFinished(EventCallback *eventData)
{
    Stop();
}

bool Bug::IsValidWord()
{
    StateVariable currentPrefix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);

    string checkWord = currentPrefix.stringValue;
    checkWord += suffix;

    return wordBuilder->IsValidWord(checkWord.c_str());
}

void Bug::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Bug::TouchStart);
    
    Sprite *splat = new Sprite(mgr);
    splat->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_SPLAT));
    splat->SetBaseLocation(pathSprite->GetBaseLocation());
    splat->AddTransformation(new AnchorTransformation());
    splat->SetLifespan(2000);
    splat->SetZIndex(-10);
    sprintf(splat->debugId, "Splat - %s", this->debugId);
    mgr->RegisterGameObject(splat);

    StateVariable points = mgr->GetStateManager()->GetVar(VAR_POINTS);
    if(IsValidWord())
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_DING);
        points.intValue++;
        StateVariable currentPrefix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);
        string correctWord = currentPrefix.stringValue;
        correctWord += suffix;
        levelSummary->AddCorrectWord(correctWord);
    }
    else
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_BUZZ);
        points.intValue -= 2;
        if(points.intValue < 0)
        {
            points.intValue = 0;
        }
        StateVariable currentPrefix = mgr->GetStateManager()->GetVar(VAR_CURRENT_PHONEME);
        string wrongWord = currentPrefix.stringValue;
        wrongWord += suffix;
        levelSummary->AddIncorrectWord(wrongWord);
    }
    mgr->GetStateManager()->SetVar(VAR_POINTS, points);

    Stop();
}