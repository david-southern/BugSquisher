#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class BugsLevel : public EventTarget
{
public:
    BugsLevel(GameManager *mgr);

    void EnterBugsLevel(EventCallback *eventData);
    void SummarizeBugsLevel(EventCallback *eventData);
    void GoToLevelSelect(EventCallback *eventData);
};
