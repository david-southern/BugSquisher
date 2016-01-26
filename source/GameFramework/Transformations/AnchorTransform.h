#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

enum AnchorPosition { AnchorPosManualPosition = 1, AnchorPosCenter, AnchorPosTopLeft, AnchorPosTopRight, AnchorPosBottomLeft, AnchorPosBottomRight };

class AnchorTransformation : public ITransformation
{
    AnchorPosition anchorPosition;
    CIwVec2 manualOffset;
    ILocationSource *manualOffsetSource;

public:
    /// The default AnchorTransformation constructor positions the sprite's anchor point at the center of the sprite
    AnchorTransformation(AnchorPosition anchorPos = AnchorPosCenter);
    AnchorTransformation(CIwVec2 manualOffset);
    AnchorTransformation(ILocationSource *_manualOffsetSource);
    ~AnchorTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


