#include "GameVariables.h"
#include "LevelSelection.h"

LevelSelection::LevelSelection(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_LEVEL_SELECT, this, (EventCallbackPtr)&LevelSelection::EnterLevelSelection);
}

void LevelSelection::GoToState(EventCallback *eventData)
{
    mgr->GetStateManager()->ChangeState((uint32)eventData->subscriberData);
}

void LevelSelection::EnterLevelSelection(EventCallback *eventData)
{
    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_LEVEL_SELECT));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "LevelSelectionBackground");

    int width = mgr->GetSurfaceWidth();
    int height = mgr->GetSurfaceHeight();

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_PLAY_BUGS_BUTTON));
    sprite->SetBaseLocation(CIwVec2(width - 10, height - 170));
    sprite->AddTransformation(new AnchorTransformation(AnchorPosBottomRight));
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "StartBugsButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&LevelSelection::GoToState, (void *)STATE_BUGS_PLAYING);

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_PLAY_APPLES_BUTTON));
    sprite->SetBaseLocation(CIwVec2(320, 160));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "StartApplesButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&LevelSelection::GoToState, (void *)STATE_APPLES_PLAYING);

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_PLAY_SANDBOX_BUTTON));
    sprite->SetBaseLocation(CIwVec2(170, 520));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "StartSandboxButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&LevelSelection::GoToState, (void *)STATE_SANDBOX_PLAYING);

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_PLAY_WALL_BUTTON));
    sprite->SetBaseLocation(CIwVec2(525, 560));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "StartWallButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&LevelSelection::GoToState, (void *)STATE_WALL_PLAYING);

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_MAIN_MENU_BUTTON));
    sprite->SetBaseLocation(CIwVec2(width / 2, height / 2));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "MainMenuButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&LevelSelection::GoToState, (void *)STATE_MAIN_MENU);

}