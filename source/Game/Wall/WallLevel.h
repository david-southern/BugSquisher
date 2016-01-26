#pragma once

#include <vector>

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class BrickTarget;
class Brick;

#define NUM_COURSES 3
#define NUM_POSITIONS 6

class WallLevel : public EventTarget
{
    BrickTarget *brickTargets[NUM_POSITIONS];
    Brick *bricks[NUM_POSITIONS];
    int brickCourses[NUM_POSITIONS];

    BrickTarget *selectedTarget;
    Brick *selectedBrick;

    void CheckValidWord();

public:
    WallLevel(GameManager *mgr);
    ~WallLevel();

    void EnterWallLevel(EventCallback *eventData);
    void SummarizeWallLevel(EventCallback *eventData);
    void GoToLevelSelect(EventCallback *eventData);

    void BrickTargetTouched(BrickTarget *target);
    void BrickTouched(Brick *brick);
    void BrickPlaced(BrickTarget *target, Brick *brick);
    void BrickPopped(BrickTarget *target, Brick *brick);
    void FillTargets();
};
