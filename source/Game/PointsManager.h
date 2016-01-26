#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class PointsManager : public TextSprite
{
    int currentPoints;
    uint32 currentSecond;
    uint32 endGameState;

public:
    PointsManager(GameManager *mgr, uint32 _endGameState);
    void AdjustPoints(EventCallback *eventData);
    void RenderPoints();
    void UpdateClock(EventCallback *eventData);
};
