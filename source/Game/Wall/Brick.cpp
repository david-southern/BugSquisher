#include <cmath>

#include "GameVariables.h"
#include "Brick.h"
#include "WallLevel.h"

int brickCount = 0;

Brick::Brick(GameManager *mgr, WallLevel *_parent, int _xPos) : Sprite(mgr)
{
    DEBUG_TRACER(Brick::Brick);

    suffix = wordBuilder->GetNextSuffix();
    parent = _parent;
    isSelected = false;
    xPos = _xPos;
    target = null;
    staticBrick = false;

    sprintf(debugId, "Brick - %d(%s) [%d]", brickCount, suffix.c_str(), _xPos);

    imageHandle = IMAGE_BRICK1;

    switch(brickCount % 3)
    {
    case 1:
        imageHandle = IMAGE_BRICK2;
        break;
    case 2:
        imageHandle = IMAGE_BRICK3;
        break;
    }

    SetBaseLocation(CIwVec2(90 + xPos * 155, 580));

    SetImage(mgr->GetResourceManager()->GetImage(imageHandle));

    AddTransformation(new AnchorTransformation());

    mgr->RegisterGameObject(this);

    suffixSprite = new TextSprite(mgr, suffix.c_str());
    suffixSprite->SetParent(this);
    suffixSprite->AddTransformation(new AnchorTransformation());
    suffixSprite->AddTransformation(new RotationTransformation(0, Clockwise));
    suffixSprite->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_18));
    suffixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    suffixSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    suffixSprite->SetBorderThickness(CIwVec2(2, 2));
    suffixSprite->SetPadding(CIwVec2(3, 3));
    suffixSprite->SetZIndex(10);
    mgr->RegisterGameObject(suffixSprite);

    sprintf(suffixSprite->debugId, "Suffix - %d(%s)", brickCount, suffix.c_str());

    brickCount++;
}


Brick::Brick(GameManager *mgr, WallLevel *_parent, int _xPos, int _yPos) : Sprite(mgr)
{
    DEBUG_TRACER(Brick::Brick);

    suffix = "";
    parent = _parent;
    isSelected = false;
    target = null;
    xPos = _xPos;
    staticBrick = true;

    sprintf(debugId, "StaticBrick - %d", brickCount);

    int imageHandle = IMAGE_BRICK1;

    switch(brickCount % 3)
    {
    case 1:
        imageHandle = IMAGE_BRICK2;
        break;
    case 2:
        imageHandle = IMAGE_BRICK3;
        break;
    }

    int brickLocX = 85 + (_yPos % 2) * 70 + xPos * 140;
    int brickLocY =  330 - 73 * _yPos;

    SetBaseLocation(CIwVec2(brickLocX, brickLocY));

    SetImage(mgr->GetResourceManager()->GetImage(imageHandle));

    AddTransformation(new AnchorTransformation());

    mgr->RegisterGameObject(this);

    brickCount++;
}

void Brick::Stop()
{
    Sprite::Stop();

    if(suffixSprite != null)
    {
        suffixSprite->Stop();
    }

    parent->BrickPopped(target, this);
}

string Brick::GetSuffix()
{
    return suffix;
}

void Brick::SetSelected(bool selected)
{
    isSelected = selected;

    suffixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    suffixSprite->ClearTransformations();

    suffixSprite->AddTransformation(new AnchorTransformation());
    suffixSprite->AddTransformation(new RotationTransformation(0, Clockwise));

    if(isSelected)
    {
        suffixSprite->AddTransformation(new ScaleTransformation(1.4f));
        suffixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_YELLOW]);
    }
}

void Brick::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Brick::TouchStart);

    if(staticBrick)
    {
        return;
    }

    parent->BrickTouched(this);
}

void Brick::PlaceBrick(EventCallback *eventData)
{
    PathFollowingSprite *path = (PathFollowingSprite *)eventData->subscriberData;
    SetBaseLocation(path->GetBaseLocation());
    SetParent(null);
    suffixSprite->Stop();
    suffixSprite = null;

    parent->BrickPlaced(target, this);
}

