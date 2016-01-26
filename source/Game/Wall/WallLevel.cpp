#include "GameVariables.h"
#include "WallLevel.h"
#include "PhonemeManager.h"
#include "PointsManager.h"
#include "Brick.h"
#include "BrickTarget.h"

#define BAD_BRICK_DELAY_MILLIS 4000

WallLevel::WallLevel(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_WALL_PLAYING, this, (EventCallbackPtr)&WallLevel::EnterWallLevel);
    mgr->GetStateManager()->SubscribeStateEntry(STATE_WALL_SUMMARY, this, (EventCallbackPtr)&WallLevel::SummarizeWallLevel);
}

WallLevel::~WallLevel()
{
}

void WallLevel::EnterWallLevel(EventCallback *eventData)
{
    mgr->RemoveAllGameObjects();

    selectedBrick = null;
    selectedTarget = null;

    StateVariable zeroVar;
    zeroVar.intValue = 0;
    mgr->GetStateManager()->SetVar(VAR_POINTS, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, zeroVar);

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_WALL_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "WallLevelBackground");

    PointsManager *pointsManager = new PointsManager(mgr, STATE_WALL_SUMMARY);

    pointsManager->ClearTransformations();
    pointsManager->AddTransformation(new AnchorTransformation(AnchorPosTopLeft));
    pointsManager->SetBaseLocation(CIwVec2(20, 20));

    for(int y = 0; y < NUM_COURSES; y++)
    {
        new Brick(mgr, this, -1, y);
        new Brick(mgr, this, NUM_POSITIONS, y);
    }

    //    for(int y=0; y < NUM_COURSES; y++)
    //    {
    //    for(int x = 0; x < NUM_POSITIONS; x++)
    //    {
    //        brickTargets[x] = new BrickTarget(mgr, this, x, y);
    //    }
    //    }
    //return;

    for(int x = 0; x < NUM_POSITIONS; x++)
    {
        brickCourses[x] = -1;
        brickTargets[x] = new BrickTarget(mgr, this, x, 0);
        bricks[x] = new Brick(mgr, this, x);
    }

}

void WallLevel::GoToLevelSelect(EventCallback *eventData)
{
    if(eventData->gameTime > (uint32)eventData->subscriberData)
    {
        mgr->GetStateManager()->ChangeState(STATE_LEVEL_SELECT);
    }
}

void WallLevel::SummarizeWallLevel(EventCallback *eventData)
{
    StateVariable finalPoints = mgr->GetStateManager()->GetVar(VAR_POINTS);

    char stateSummary[1024];

    sprintf(stateSummary, "You got %d points.\nHave some stars!\nTouch to continue.", finalPoints.intValue);

    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_WALL_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "WallLevelBackground");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&WallLevel::GoToLevelSelect, (void*)(mgr->GetCurrentClockTime() + 2000));

    for(int y = 0; y < NUM_COURSES; y++)
    {
        for(int x = -1; x<= NUM_POSITIONS; x++)
        {
            new Brick(mgr, this, x, y);
        }
    }

    TextSprite *summary = new TextSprite(mgr, stateSummary);
    summary->SetAlign(IW_2D_FONT_ALIGN_CENTRE);
    summary->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    summary->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_24));
    summary->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    summary->SetBorderThickness(CIwVec2(4, 4));
    summary->SetPadding(CIwVec2(7, 7));
    summary->AddTransformation(new AnchorTransformation());
    summary->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    summary->SetZIndex(999);
    mgr->RegisterGameObject(summary);
}

void WallLevel::BrickTouched(Brick *whichBrick)
{
    selectedBrick = (selectedBrick == whichBrick) ? null : whichBrick;

    CheckValidWord();

    for(uint32 index = 0; index < NUM_POSITIONS; index++)
    {
        Brick *updateBrick = bricks[index];
        if(updateBrick == null)
        {
            continue;
        }
        updateBrick->SetSelected(updateBrick == selectedBrick);
    }
}

void WallLevel::BrickTargetTouched(BrickTarget *whichBrickTarget)
{
    selectedTarget = (selectedTarget == whichBrickTarget) ? null : whichBrickTarget;

    CheckValidWord();

    for(uint32 index = 0; index < NUM_POSITIONS; index++)
    {
        BrickTarget *updateBrickTarget = brickTargets[index];
        if(updateBrickTarget == null)
        {
            continue;
        }
        updateBrickTarget->SetSelected(updateBrickTarget == selectedTarget);
    }
}

void WallLevel::CheckValidWord()
{
    if(selectedTarget == null || selectedBrick == null)
    {
        return;
    }

    string checkWord = selectedTarget->GetPrefix();
    checkWord += selectedBrick->GetSuffix();

    StateVariable points = mgr->GetStateManager()->GetVar(VAR_POINTS);
    if(wordBuilder->IsValidWord(checkWord.c_str()))
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_DING);
        points.intValue++;
    }
    else
    {
        mgr->GetResourceManager()->PlayAudio(SOUND_BUZZ);
        points.intValue -= 2;
        if(points.intValue < 0)
        {
            points.intValue = 0;
        }
    }
    mgr->GetStateManager()->SetVar(VAR_POINTS, points);

    CIwFVec2 curLocation((float)selectedBrick->GetBaseLocation().x, (float)selectedBrick->GetBaseLocation().y);
    CIwFVec2 targetLocation((float)selectedTarget->GetBaseLocation().x, (float)selectedTarget->GetBaseLocation().y);

    float orientation = IW_ANGLE_TO_RADIANS(0);

    CIwFVec2 normal;
    normal.x = (float)cos(orientation);
    normal.y = (float)sin(orientation);

    CIwFVec2 cp1 = curLocation + normal * 120;
    CIwFVec2 cp2 =  (targetLocation - curLocation) / 2;
    cp2 += curLocation + cp2 + normal * cp2.GetLength();

    PathFollowingSprite *path = new PathFollowingSprite(mgr);

    path->SetTotalPathTimeMillis(2000);
    path->AddSegment(new PathSegmentBezier(Convert(curLocation), Convert(cp1), Convert(cp2), Convert(targetLocation)));
    path->SetCompletionCallback(selectedBrick, (EventCallbackPtr)&Brick::PlaceBrick, path);
    path->SetLoopCount(1);
    mgr->RegisterGameObject(path);

    selectedBrick->SetParent(path);
    selectedBrick->target = selectedTarget;
    selectedTarget->Hide();

    selectedBrick->staticBrick = true;
    selectedTarget->staticTarget = true;

    bricks[selectedBrick->xPos] = null;

    s3eDebugTracePrintf("%d - Pathing brick %s", mgr->GetCurrentClockTime(), selectedBrick->debugId);

    selectedBrick = null;
    selectedTarget = null;
}

#define SHAKER_FRAME_TIME_MILLIS 100

class ShakerLocSource : public ILocationSource
{
    uint32 nextChangeTime;
    int deltaX, deltaY;

public:
    ShakerLocSource() : ILocationSource()
    {
        nextChangeTime = 0;
    }

    CIwVec2 GetLocation(uint32 gameTime)
    {
        if(gameTime > nextChangeTime)
        {
            nextChangeTime = gameTime + SHAKER_FRAME_TIME_MILLIS;
            deltaX = rand() % 11 - 5;
            deltaY = rand() % 11 - 5;
        }

        return CIwVec2(deltaX, deltaY);
    }
};


void WallLevel::BrickPlaced(BrickTarget *target, Brick *brick)
{
    string checkWord = target->GetPrefix();
    checkWord += brick->GetSuffix();

    s3eDebugTracePrintf("%d - Placing brick %s at target %s", mgr->GetCurrentClockTime(), brick->debugId, target->debugId);
    brick->SetBaseLocation(target->GetBaseLocation());

    if(wordBuilder->IsValidWord(checkWord.c_str()))
    {
        brickCourses[target->xPos] = target->yPos;
        if(bricks[brick->xPos] == null)
        {
            bricks[brick->xPos] = new Brick(mgr, this, brick->xPos);
        }
        brickTargets[target->xPos] = null;
        target->Stop();
        FillTargets();
    }
    else
    {
        brick->AddTransformation(new TranslationTransformation(new ShakerLocSource));
        uint32 shakeEndTime = brick->GetAge(mgr->GetCurrentClockTime()) + BAD_BRICK_DELAY_MILLIS;
        brick->SetLifespan(shakeEndTime);
        s3eDebugTracePrintf("Shaking brick %s until time %d", brick->debugId, shakeEndTime);
    }
}

void WallLevel::BrickPopped(BrickTarget *target, Brick *brick)
{
    brickCourses[target->xPos] = target->yPos - 1;
    if(bricks[brick->xPos] == null)
    {
        bricks[brick->xPos] = new Brick(mgr, this, brick->xPos);
    }
    brickTargets[target->xPos] = null;
    target->Stop();
    FillTargets();
}

void WallLevel::FillTargets()
{
    bool wallFull = true;

    for(uint32 x = 0; x < NUM_POSITIONS; x++)
    {
        if(brickTargets[x] == null)
        {
            bool allowBrick = false;
            int targetCourse = brickCourses[x] + 1;

            if(targetCourse < NUM_COURSES)
            {
                wallFull = false;
            }

            switch(targetCourse)
            {
            case 0:
                allowBrick = true;
                break;

            case 1: 
                if(x == NUM_POSITIONS - 1 || brickCourses[x] <= brickCourses[x + 1])
                {
                    allowBrick = true;
                }
                break;

            case 2:
                if(x == 0 || brickCourses[x] <= brickCourses[x - 1])
                {
                    allowBrick = true;
                }
                break;

            default:
                allowBrick = false;
            }

            if(!allowBrick)
            {
                continue;
            }

            brickTargets[x] = new BrickTarget(mgr, this, x, targetCourse);
        }
        else
        {
            wallFull = false;
        }
    }

    if(wallFull)
    {
        StateVariable points = mgr->GetStateManager()->GetVar(VAR_POINTS);
        points.intValue += 10;
        mgr->GetStateManager()->SetVar(VAR_POINTS, points);
        mgr->GetStateManager()->ClearFutureEvents();
        mgr->GetStateManager()->ChangeState(STATE_WALL_SUMMARY);
    }
}