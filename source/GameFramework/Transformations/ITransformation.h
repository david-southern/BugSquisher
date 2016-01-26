#pragma once

#include "s3e.h"
#include "Iw2D.h"

#include "Constants.h"

class Sprite;

enum RotationDirection { Clockwise = 1, CounterClockwise };
enum OrientationDirection { None = 1, OrientToward, OrientAway, OrientAlongTangent, OrientAlongNormal, OrientReverseTangent, OrientReverseNormal, OrientAlongOrientation, OrientReverseOrientation };

const iwangle ANGLE_90_DEGREES = IW_ANGLE_FROM_DEGREES(90);
const iwangle ANGLE_NEG90_DEGREES = IW_ANGLE_FROM_DEGREES(270);
const iwangle ANGLE_180_DEGREES = IW_ANGLE_FROM_DEGREES(180);

#define ANGLE_ADDITION(a, b) IW_ANGLE_MASK_PHASE(a + b);
#define ANGLE_SUBTRACTION(a, b) IW_ANGLE_MASK_PHASE(a - b);
#define ANGLE_MULTIPLICATION(angle, factor) IW_ANGLE_MASK_PHASE(IW_FIXED_MUL(angle, factor))
class TransformationState
{
public:
    uint32 gameTime;
    Sprite* sprite;
    CIwVec2 originalLocation;
    CIwVec2 currentLocation;
    CIwVec2 anchorPoint;
    iwangle orientation;
    iwangle tangent;
    iwangle normal;
    iwfixed scaleFactor;
};

class ITransformation
{
public:
    virtual ~ITransformation() {}

    virtual void ApplyTransformation(TransformationState& state) = 0;
};


