#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "Sprite.h"

class FlipbookSprite : public Sprite
{
    uint32 frameHeight;
    uint32 frameWidth;

    CIwVec2 imageOffset;
    uint32 numFrames;
    uint32 animationFPS;

    uint32 millisPerFrame;

    virtual void RenderImage(uint32 gameTime);

public:
    FlipbookSprite(GameManager *gameManager);

    void SetAnimationParams(CIw2DImage *image, uint32 frameWidth, uint32 frameHeight, uint32 numFrames, uint32 animationFPS, CIwVec2 imageOffset = CIwVec2(0, 0));

    uint32 GetFrameWidth();
    uint32 GetFrameHeight();
    uint32 GetNumFrames();
    uint32 GetAnimationFPS();
    CIwVec2 GetImageOffset();

    int32 GetWidth();
    int32 GetHeight();
};


