#include "s3e.h"
#include "Iw2D.h"

#include "Sprite.h"
#include "AnchorTransform.h"

AnchorTransformation::AnchorTransformation(AnchorPosition anchorPos)
{
    anchorPosition = anchorPos;
    manualOffset = CIwVec2(0, 0);
    manualOffsetSource = null;
}

AnchorTransformation::AnchorTransformation(CIwVec2 manualOffset)
{
    anchorPosition = AnchorPosManualPosition;
    this->manualOffset = manualOffset;
    manualOffsetSource = null;
}

AnchorTransformation::AnchorTransformation(ILocationSource *_manualOffsetSource)
{
    anchorPosition = AnchorPosManualPosition;
    manualOffsetSource = _manualOffsetSource;
}

AnchorTransformation::~AnchorTransformation()
{
    if(manualOffsetSource != null && manualOffsetSource->locSourceNeedsDelete)
    {
        delete manualOffsetSource;
    }
}

void AnchorTransformation::ApplyTransformation(TransformationState& state)
{
    CIwVec2 offset = manualOffset;

    if(manualOffsetSource != null)
    {
        offset = manualOffsetSource->GetLocation(state.gameTime);
    }

    switch(anchorPosition)
    {
    case AnchorPosCenter:
        offset.x = state.sprite->GetWidth() / 2;
        offset.y = state.sprite->GetHeight() / 2;
        break;
    case AnchorPosTopLeft:
        offset.x = 0;
        offset.y = 0;
        break;
    case AnchorPosTopRight:
        offset.x = state.sprite->GetWidth();
        offset.y = 0;
        break;
    case AnchorPosBottomLeft:
        offset.x = 0;
        offset.y = state.sprite->GetHeight();
        break;
    case AnchorPosBottomRight:
        offset.x = state.sprite->GetWidth();
        offset.y = state.sprite->GetHeight();
        break;
    case AnchorPosManualPosition:
        break;
    }

    state.anchorPoint = SCREEN_TO_SUBPIXEL(offset);
}
