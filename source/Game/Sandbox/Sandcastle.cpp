#include "GameVariables.h"
#include "Sandcastle.h"

#define GOAL_POINTS 20

Sandcastle::Sandcastle(GameManager *mgr) : Sprite(mgr)
{
    currentPoints = 0;

    mgr->RegisterGameObject(this);

    SetImage(mgr->GetResourceManager()->GetImage(IMAGE_SANDCASTLE));
    AddTransformation(new AnchorTransformation(AnchorPosBottomLeft));
    SetBaseLocation(CIwVec2(250, 320));
    SetZIndex(20);

    strcpy(this->debugId, "Sandcastle");

    mgr->GetStateManager()->SubscribeStateVarChange(VAR_POINTS, this, (EventCallbackPtr)&Sandcastle::AdjustPoints);
}

void Sandcastle::AdjustPoints(EventCallback *eventData)
{
    VarEventData *varData = (VarEventData *)eventData->eventData;
    currentPoints = varData->newValue.intValue;

    s3eDebugTracePrintf("Sandcastle current points: %d", currentPoints);
}

uint32 lastReport = 0;

void Sandcastle::RenderImage(uint32 gameTime)
{
    float pctComplete = (float)currentPoints / GOAL_POINTS;

    if(pctComplete > 1.0)
    {
        pctComplete = 1.0;
    }

    CIwSVec2 imgOffset, imgSize;

    imgSize.x = GetWidth();
    imgSize.y = (int)(GetHeight() * pctComplete);

    if(imgSize.y < 1)
    {
        return;
    }

    imgOffset.x = 0;
    imgOffset.y = GetHeight() - imgSize.y;

    Iw2DDrawImageRegion(image, SCREEN_TO_SUBPIXEL(imgOffset), imgOffset, imgSize);
}
