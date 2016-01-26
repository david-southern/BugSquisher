#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

class ScaleTransformation : public ITransformation
{
    iwfixed scaleFactor;
    IFloatSource *scaleFactorSource;

public:
    ScaleTransformation(float scaleFactor);
    ScaleTransformation(IFloatSource *floatSource);
    ~ScaleTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


