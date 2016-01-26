#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class LevelSelection : public EventTarget
{

public:
    LevelSelection(GameManager *mgr);

    void EnterLevelSelection(EventCallback *eventData);
    void GoToState(EventCallback *eventData);    
};
