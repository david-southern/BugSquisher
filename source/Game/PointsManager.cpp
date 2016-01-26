#include "GameVariables.h"
#include "PointsManager.h"

#define GAME_LENGTH_SECONDS 30

// Really we only need to update the timer once per second, so our timer delay should be 1000, however
// the game clock isn't perfectly accurate, so our event tick will "precess" away from a perfect 
// interval that we request.  This will result in the timer either "skipping" a tick of a number, or 
// staying one the same number for a double-tick.  If the tick is a full second, then this looks bad.
// Even if it is 1/2 second, it is still noticable.  1/10 of a second shouldn't be too visible
#define TIMER_UPDATE_TICK 100

PointsManager::PointsManager(GameManager *mgr, uint32 _endGameState) : TextSprite(mgr, "")
{
    endGameState = _endGameState;

    currentPoints = 0;
    currentSecond = 0;

    mgr->RegisterGameObject(this);

    AddTransformation(new AnchorTransformation(AnchorPosTopRight));
    SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() - 20, 20));
    SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_24));
    SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    SetBorderThickness(CIwVec2(4, 4));
    SetPadding(CIwVec2(8, 8));

    strcpy(this->debugId, "PointsManager");

    mgr->GetStateManager()->SubscribeStateVarChange(VAR_POINTS, this, (EventCallbackPtr)&PointsManager::AdjustPoints);
    mgr->GetStateManager()->SubscribeEvent("UpdateClock", this, (EventCallbackPtr)&PointsManager::UpdateClock);
    mgr->GetStateManager()->PostFutureEvent("UpdateClock", mgr->GetCurrentClockTime() + TIMER_UPDATE_TICK);

    RenderPoints();
}

void PointsManager::AdjustPoints(EventCallback *eventData)
{
    VarEventData *varData = (VarEventData *)eventData->eventData;
    currentPoints = varData->newValue.intValue;
    RenderPoints();
}

void PointsManager::RenderPoints()
{
    char buffer[200];
    sprintf(buffer, "Time: %d", GAME_LENGTH_SECONDS - currentSecond);
    SetText(buffer);
}

void PointsManager::UpdateClock(EventCallback *eventData)
{
    // If I'm not actibe because I haven't been born yet then go ahead and continue, all that means
    // is that the setup of the level took longer than TIMER_UPDATE_TICK allows, so this event is being
    // received immediately after the PointsManager was created
    if(!Active() && birthMillis > 0)
    {
        return;
    }

    currentSecond = GetAge(eventData->gameTime) / 1000;
    RenderPoints();

    if(currentSecond > GAME_LENGTH_SECONDS)
    {
        mgr->GetStateManager()->ClearFutureEvents();
        mgr->GetStateManager()->ChangeState(endGameState);
    }
    else
    {
        mgr->GetStateManager()->PostFutureEvent("UpdateClock", mgr->GetCurrentClockTime() + TIMER_UPDATE_TICK);
    }
}
