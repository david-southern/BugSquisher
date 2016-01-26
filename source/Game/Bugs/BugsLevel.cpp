#include "GameVariables.h"
#include "BugsLevel.h"
#include "PhonemeManager.h"
#include "PointsManager.h"
#include "Bug.h"

int BUG_COUNT = 7;
int BUG_SPEED = 75;

BugsLevel::BugsLevel(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_BUGS_PLAYING, this, (EventCallbackPtr)&BugsLevel::EnterBugsLevel);
    mgr->GetStateManager()->SubscribeStateEntry(STATE_BUGS_SUMMARY, this, (EventCallbackPtr)&BugsLevel::SummarizeBugsLevel);
}

void BugsLevel::EnterBugsLevel(EventCallback *eventData)
{
    levelSummary->Reset();

    mgr->RemoveAllGameObjects();

    StateVariable zeroVar;
    zeroVar.intValue = 0;
    mgr->GetStateManager()->SetVar(VAR_POINTS, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, zeroVar);

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_BUGS_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "BugsLevelBackground");

    // These IGameOjects register themselves to the GameManager, which takes care of freeing them.
    // No memory leak here
    new PhonemeManager(mgr, true);

    for(int bugIndex = 0; bugIndex < BUG_COUNT; bugIndex++)
    {
        // These IGameOjects register themselves to the GameManager, which takes care of freeing them.
        // No memory leak here
        new Bug(mgr);
    }
    
    new PointsManager(mgr, STATE_BUGS_SUMMARY);

}

void BugsLevel::GoToLevelSelect(EventCallback *eventData)
{
    if(eventData->gameTime > (uint32)eventData->subscriberData)
    {
        mgr->GetStateManager()->ChangeState(STATE_LEVEL_SELECT);
    }
}

void BugsLevel::SummarizeBugsLevel(EventCallback *eventData)
{
    StateVariable finalPoints = mgr->GetStateManager()->GetVar(VAR_POINTS);

    char stateSummary[1024];

    sprintf(stateSummary, "You got %d points.\nHave some stars!\nTouch to continue.", finalPoints.intValue);

    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_BUGS_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "BugsLevelBackground");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&BugsLevel::GoToLevelSelect, (void*)(mgr->GetCurrentClockTime() + 2000));

    levelSummary->Display();

    /*
    TextSprite *summary = new TextSprite(mgr, stateSummary);
    summary->SetAlign(IW_2D_FONT_ALIGN_CENTRE);
    summary->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    summary->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_24));
    summary->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    summary->SetBorderThickness(CIwVec2(4, 4));
    summary->SetPadding(CIwVec2(7, 7));
    summary->AddTransformation(new AnchorTransformation());
    summary->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    mgr->RegisterGameObject(summary);
    */
}