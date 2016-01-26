#include "GameVariables.h"
#include "SandboxLevel.h"
#include "PhonemeManager.h"
#include "PointsManager.h"
#include "Dumptruck.h"
#include "Sandcastle.h"

int DUMPTRUCK_DELAY = 1500;

SandboxLevel::SandboxLevel(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_SANDBOX_PLAYING, this, (EventCallbackPtr)&SandboxLevel::EnterSandboxLevel);
    mgr->GetStateManager()->SubscribeStateEntry(STATE_SANDBOX_SUMMARY, this, (EventCallbackPtr)&SandboxLevel::SummarizeSandboxLevel);
    mgr->GetStateManager()->SubscribeEvent(EVENT_NEW_DUMPTRUCK, this, (EventCallbackPtr)&SandboxLevel::NewDumptruck);
}

void SandboxLevel::EnterSandboxLevel(EventCallback *eventData)
{
    mgr->RemoveAllGameObjects();

    StateVariable zeroVar;
    zeroVar.intValue = 0;
    mgr->GetStateManager()->SetVar(VAR_POINTS, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, zeroVar);

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_SANDBOX_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "SandboxLevelBackground");

    // These IGameOjects register themselves to the GameManager, which takes care of freeing them.
    // No memory leak here
    PhonemeManager *phonemeMgr = new PhonemeManager(mgr, false);
    phonemeMgr->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() - 110, 110));

    new Sandcastle(mgr);

    PointsManager *pointsManager = new PointsManager(mgr, STATE_SANDBOX_SUMMARY);

    pointsManager->ClearTransformations();
    pointsManager->AddTransformation(new AnchorTransformation(AnchorPosTopLeft));
    pointsManager->SetBaseLocation(CIwVec2(20, 20));

    mgr->GetStateManager()->PostFutureEvent(EVENT_NEW_DUMPTRUCK, mgr->GetCurrentClockTime());
}

void SandboxLevel::NewDumptruck(EventCallback *eventData)
{
    new Dumptruck(mgr);
    mgr->GetStateManager()->PostFutureEvent(EVENT_NEW_DUMPTRUCK, eventData->gameTime + DUMPTRUCK_DELAY);
}

void SandboxLevel::GoToLevelSelect(EventCallback *eventData)
{
    if(eventData->gameTime > (uint32)eventData->subscriberData)
    {
        mgr->GetStateManager()->ChangeState(STATE_LEVEL_SELECT);
    }
}

void SandboxLevel::SummarizeSandboxLevel(EventCallback *eventData)
{
    StateVariable finalPoints = mgr->GetStateManager()->GetVar(VAR_POINTS);

    char stateSummary[1024];

    sprintf(stateSummary, "You got %d points.\nHave some stars!\nTouch to continue.", finalPoints.intValue);

    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_SANDBOX_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "SandboxLevelBackground");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&SandboxLevel::GoToLevelSelect, (void*)(mgr->GetCurrentClockTime() + 2000));

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
}