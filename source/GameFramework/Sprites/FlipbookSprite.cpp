#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "FlipbookSprite.h"

FlipbookSprite::FlipbookSprite(GameManager *gameManager) : Sprite(gameManager)
{
}

void FlipbookSprite::SetAnimationParams(CIw2DImage *image, uint32 frameWidth, uint32 frameHeight, uint32 numFrames, uint32 animationFPS, CIwVec2 imageOffset)
{
    SetImage(image);
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    this->numFrames = numFrames;
    this->animationFPS = animationFPS;
    this->imageOffset = imageOffset;

    uint32 imageWidth = image->GetWidth();
    uint32 imageHeight = image->GetHeight();

    if(imageWidth < imageOffset.x + frameWidth * numFrames)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Flipbook image is too narrow for settings");
    }

    if(imageHeight < imageOffset.y + frameHeight)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Flipbook image is too short for settings");
    }

    millisPerFrame = 1000 / animationFPS;
}

uint32 FlipbookSprite::GetFrameWidth()
{
    return frameWidth;

}

uint32 FlipbookSprite::GetFrameHeight()
{
    return frameHeight;
}

uint32 FlipbookSprite::GetNumFrames()
{
    return numFrames;
}

uint32 FlipbookSprite::GetAnimationFPS()
{
    return animationFPS;
}

CIwVec2 FlipbookSprite::GetImageOffset()
{
    return imageOffset;
}

int32 FlipbookSprite::GetWidth()
{
    return frameWidth;
}

int32 FlipbookSprite::GetHeight()
{
    return frameHeight;
}

void FlipbookSprite::RenderImage(uint32 gameTime)
{
    uint32 frame = (GetAge(gameTime) / millisPerFrame) % numFrames;

    CIwSVec2 frameOffset = CIwSVec2(imageOffset + CIwVec2(frame * frameWidth, 0));
    CIwSVec2 frameSize(frameWidth, frameHeight);

    Iw2DDrawImageRegion(image, CIwSVec2(0, 0), frameOffset, frameSize);
}

