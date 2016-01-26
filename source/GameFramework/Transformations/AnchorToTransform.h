#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "ITransformation.h"
#include "AnimatableParameters.h"

enum AnchorType { RelativeToAnchorPoint = 1, RelativeToOrientation, RelativeToTangent, RelativeToNormal };

class AnchorToTransformation : public ITransformation
{
    ILocationSource *locSource;
    IDirectionalSource *orientationSource;
    CIwVec2 offset;
    ILocationSource *offsetSource;
    iwfixed radius;
    IFloatSource *radiusSource;
    iwangle orientationOffset;
    IFloatSource *orientationOffsetSource;
    AnchorType anchorType;

    void Initialize();
    void CalculateOffset(iwangle orientation);

public:
    AnchorToTransformation(ILocationSource *_locSource, CIwVec2 _offset);
    AnchorToTransformation(ILocationSource *_locSource, ILocationSource *_offsetSource);
    AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource, float _radius, float _orientationOffsetDegrees, AnchorType _anchorType = RelativeToOrientation);
    AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource, IFloatSource *_radiusSource, float _orientationOffsetDegrees, AnchorType _anchorType = RelativeToOrientation);
    AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource, float _radius, IFloatSource *_orientationOffsetDegreesSource, AnchorType _anchorType = RelativeToOrientation);
    AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource, IFloatSource *_radiusSource, IFloatSource *_orientationOffsetDegreesSource, AnchorType _anchorType = RelativeToOrientation);

    ~AnchorToTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


