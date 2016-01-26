#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class MainMenu : public EventTarget
{

public:
    MainMenu(GameManager *mgr);

    void EnterMainMenu(EventCallback *eventData);
    void GoToLevelSelect(EventCallback *eventData);
};
