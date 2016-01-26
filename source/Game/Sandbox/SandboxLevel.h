#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class SandboxLevel : public EventTarget
{
public:
    SandboxLevel(GameManager *mgr);

    void EnterSandboxLevel(EventCallback *eventData);
    void SummarizeSandboxLevel(EventCallback *eventData);
    void GoToLevelSelect(EventCallback *eventData);

    void NewDumptruck(EventCallback *eventData);
};
