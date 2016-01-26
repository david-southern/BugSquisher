#pragma once

#define MAX_ORBIT_TIME_MILLIS 300000

#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

class OrbitTransformation : public ITransformation, public IDirectionalSource
{
    int32 radius;
    IIntSource *radiusSource;

    uint32 orbitTimeMillis;
    iwangle initialAngle;
    RotationDirection rotationDirection;
    OrientationDirection orientationDirection;
    uint32 birthdayMillis;

    iwangle GetAngle(uint32 gameTime);

public:
    OrbitTransformation(int32 radius, uint32 orbitTimeMillis, float initialAngle = 0, RotationDirection rotationDirection = Clockwise, 
        OrientationDirection orientationDirection = OrientAlongTangent);
    OrbitTransformation(IIntSource *_radiusSource, uint32 orbitTimeMillis, float initialAngle = 0, RotationDirection rotationDirection = Clockwise, 
        OrientationDirection orientationDirection = OrientAlongTangent);
    ~OrbitTransformation();

    virtual void ApplyTransformation(TransformationState& state);
    virtual iwangle GetNormal(uint32 gameTime);
    virtual iwangle GetTangent(uint32 gameTime);
    virtual iwangle GetOrientation(uint32 gameTime);
};


