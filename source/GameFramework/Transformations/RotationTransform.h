#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

class RotationTransformation : public ITransformation
{
    iwangle rotation;
    RotationDirection direction;
    IFloatSource *rotationSource;

    void Initialize(float _rotationDegrees, RotationDirection _direction);

public:
    RotationTransformation(int rotationDegrees, RotationDirection direction);
    RotationTransformation(float rotationDegrees, RotationDirection direction);
    RotationTransformation(IFloatSource *floatSource, RotationDirection direction);
    ~RotationTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


