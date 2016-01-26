#include "GameVariables.h"
#include "ApplesLevel.h"
#include "PhonemeManager.h"
#include "PointsManager.h"
#include "Apple.h"

int APPLE_COUNT = 7;

ApplesLevel::ApplesLevel(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_APPLES_PLAYING, this, (EventCallbackPtr)&ApplesLevel::EnterApplesLevel);
    mgr->GetStateManager()->SubscribeStateEntry(STATE_APPLES_SUMMARY, this, (EventCallbackPtr)&ApplesLevel::SummarizeApplesLevel);
}

void ApplesLevel::EnterApplesLevel(EventCallback *eventData)
{
    levelSummary->Reset();

    mgr->RemoveAllGameObjects();

    StateVariable zeroVar;
    zeroVar.intValue = 0;
    mgr->GetStateManager()->SetVar(VAR_POINTS, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_THINGY_COUNT, zeroVar);
    mgr->GetStateManager()->SetVar(VAR_CURRENT_CORRECT_THINGY_COUNT, zeroVar);

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_APPLES_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "ApplesLevelBackground");

    // These IGameOjects register themselves to the GameManager, which takes care of freeing them.
    // No memory leak here
    PhonemeManager *phonemeMgr = new PhonemeManager(mgr, false);
    phonemeMgr->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() - 110, 110));

    for(int appleIndex = 0; appleIndex < APPLE_COUNT; appleIndex++)
    {
        // These IGameOjects register themselves to the GameManager, which takes care of freeing them.
        // No memory leak here
        new Apple(mgr);
    }

    PointsManager *pointsManager = new PointsManager(mgr, STATE_APPLES_SUMMARY);

    pointsManager->ClearTransformations();
    pointsManager->AddTransformation(new AnchorTransformation(AnchorPosTopLeft));
    pointsManager->SetBaseLocation(CIwVec2(20, 20));

}

void ApplesLevel::AppleDrop(EventCallback *eventData)
{
    Apple *apple = (Apple *)eventData->eventData;
    apple->AppleDrop(eventData);
}

void ApplesLevel::GoToLevelSelect(EventCallback *eventData)
{
    if(eventData->gameTime > (uint32)eventData->subscriberData)
    {
        mgr->GetStateManager()->ChangeState(STATE_LEVEL_SELECT);
    }
}

void ApplesLevel::SummarizeApplesLevel(EventCallback *eventData)
{
    StateVariable finalPoints = mgr->GetStateManager()->GetVar(VAR_POINTS);

    char stateSummary[1024];

    sprintf(stateSummary, "You got %d points.\nHave some stars!\nTouch to continue.", finalPoints.intValue);

    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_APPLES_BACKGROUND));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "ApplesLevelBackground");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&ApplesLevel::GoToLevelSelect, (void*)(mgr->GetCurrentClockTime() + 2000));

    levelSummary->Display();

    //TextSprite *summary = new TextSprite(mgr, stateSummary);
    //summary->SetAlign(IW_2D_FONT_ALIGN_CENTRE);
    //summary->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    //summary->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_24));
    //summary->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    //summary->SetBorderThickness(CIwVec2(4, 4));
    //summary->SetPadding(CIwVec2(7, 7));
    //summary->AddTransformation(new AnchorTransformation());
    //summary->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    //mgr->RegisterGameObject(summary);
}