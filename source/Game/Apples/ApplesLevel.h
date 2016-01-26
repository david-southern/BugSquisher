#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class ApplesLevel : public EventTarget
{
public:
    ApplesLevel(GameManager *mgr);

    void EnterApplesLevel(EventCallback *eventData);
    void SummarizeApplesLevel(EventCallback *eventData);
    void GoToLevelSelect(EventCallback *eventData);
    void AppleDrop(EventCallback *eventData);
};
