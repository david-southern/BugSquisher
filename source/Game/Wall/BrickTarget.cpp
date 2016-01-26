#include <cmath>

#include "GameVariables.h"
#include "BrickTarget.h"
#include "WallLevel.h"

int brickTargetCount = 0;

BrickTarget::BrickTarget(GameManager *mgr, WallLevel *_parent, int _xPos, int _yPos) : Sprite(mgr)
{
    DEBUG_TRACER(BrickTarget::BrickTarget);

    prefix = wordBuilder->GetNextPrefix();
    isSelected = false;
    parent = _parent;
    staticTarget = false;

    sprintf(debugId, "BrTg - %d(%s) [%d, %d]", brickTargetCount, prefix.c_str(), _xPos, _yPos);

    SetImage(mgr->GetResourceManager()->GetImage(IMAGE_BRICK_TARGET));

    xPos = _xPos;
    yPos = _yPos;

    int brickLocX = 85 + (yPos % 2) * 70 + xPos * 140;
    int brickLocY =  330 - 73 * yPos;

    SetBaseLocation(CIwVec2(brickLocX, brickLocY));

    AddTransformation(new AnchorTransformation());

    mgr->RegisterGameObject(this);

    //char buffer[50];
    //sprintf(buffer, "%d, %d", xPos, yPos);
    //prefixSprite = new TextSprite(mgr, buffer);

    prefixSprite = new TextSprite(mgr, prefix.c_str());
    prefixSprite->SetParent(this);
    prefixSprite->AddTransformation(new AnchorTransformation());
    prefixSprite->AddTransformation(new RotationTransformation(0, Clockwise));
    prefixSprite->SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_18));
    prefixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    prefixSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    prefixSprite->SetBorderThickness(CIwVec2(2, 2));
    prefixSprite->SetPadding(CIwVec2(3, 3));
    prefixSprite->SetZIndex(10);
    mgr->RegisterGameObject(prefixSprite);

    sprintf(prefixSprite->debugId, "Brick Target Prefix - %d(%s)", brickTargetCount, prefix.c_str());

    brickTargetCount++;
}

void BrickTarget::Stop()
{
    Sprite::Stop();

    s3eDebugTracePrintf("%d - Stopping BT: %s", mgr->GetCurrentClockTime(), debugId);
    if(prefixSprite != null)
    {
        prefixSprite->Stop();
    }
}

string BrickTarget::GetPrefix()
{
    return prefix;
}

void BrickTarget::SetSelected(bool selected)
{
    isSelected = selected;

    if(prefixSprite != null)
    {
        prefixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
        prefixSprite->ClearTransformations();

        prefixSprite->AddTransformation(new AnchorTransformation());
        prefixSprite->AddTransformation(new RotationTransformation(0, Clockwise));

        if(isSelected)
        {
            prefixSprite->AddTransformation(new ScaleTransformation(1.4f));
            prefixSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_YELLOW]);
        }
    }
}

void BrickTarget::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Brick::TouchStart);

    if(staticTarget)
    {
        return;
    }

    parent->BrickTargetTouched(this);
}

void BrickTarget::Hide()
{
    if(prefixSprite != null)
    {
        prefixSprite->Stop();
        prefixSprite = null;
    }

    SetImage(null);
}
