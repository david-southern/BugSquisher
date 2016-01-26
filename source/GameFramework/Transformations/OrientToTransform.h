#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

class OrientToTransformation : public ITransformation
{
    CIwVec2 orientLocation;
    IDirectionalSource *directional;
    ILocationSource *locationSource;
    uint16 orientDirection;

public:
    OrientToTransformation(CIwVec2 location, OrientationDirection orientDirection);
    OrientToTransformation(IDirectionalSource *directional, OrientationDirection orientDirection);
    OrientToTransformation(ILocationSource *locationSource, OrientationDirection orientDirection);

    ~OrientToTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


