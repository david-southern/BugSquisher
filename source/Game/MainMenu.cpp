#include "GameVariables.h"
#include "MainMenu.h"

MainMenu::MainMenu(GameManager *mgr) : EventTarget(mgr)
{
    mgr->GetStateManager()->SubscribeStateEntry(STATE_MAIN_MENU, this, (EventCallbackPtr)&MainMenu::EnterMainMenu);
}

void MainMenu::GoToLevelSelect(EventCallback *eventData)
{
    mgr->GetStateManager()->ChangeState(STATE_LEVEL_SELECT);
}

void MainMenu::EnterMainMenu(EventCallback *eventData)
{
    mgr->RemoveAllGameObjects();

    Sprite *sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_MAIN_MENU));
    sprite->SetBaseLocation(CIwVec2(0, 0));
    sprite->SetZIndex(-999);
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "MainMenu");

    int width = mgr->GetSurfaceWidth();
    int height = mgr->GetSurfaceHeight();

    int buttonHeight = height - 50;

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_CHANGE_PLAYER_BUTTON));
    sprite->SetBaseLocation(CIwVec2(width / 2, buttonHeight));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "ChangePlayerButton");

    buttonHeight -= (sprite->GetHeight() + 30);

    sprite = new Sprite(mgr);
    sprite->SetImage(mgr->GetResourceManager()->GetImage(IMAGE_START_GAME_BUTTON));
    sprite->SetBaseLocation(CIwVec2(width / 2, buttonHeight));
    sprite->AddTransformation(new AnchorTransformation());
    mgr->RegisterGameObject(sprite);
    strcpy(sprite->debugId, "StartGameButton");
    sprite->SetTouchEndCallback(this, (EventCallbackPtr)&MainMenu::GoToLevelSelect);

}