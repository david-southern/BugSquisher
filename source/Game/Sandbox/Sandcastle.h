#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class Sandcastle : public Sprite
{
    int currentPoints;

public:
    Sandcastle(GameManager *mgr);
    void AdjustPoints(EventCallback *eventData);
    virtual void RenderImage(uint32 gameTime);
};
