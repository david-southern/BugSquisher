#include "s3e.h"
#include "Iw2D.h"

#include "RotationTransform.h"

RotationTransformation::RotationTransformation(int rotationDegrees, RotationDirection direction)
{
    Initialize((float)rotationDegrees, direction);
}

RotationTransformation::RotationTransformation(float rotationDegrees, RotationDirection direction)
{
    Initialize(rotationDegrees, direction);
}

RotationTransformation::RotationTransformation(IFloatSource *floatSource, RotationDirection direction)
{
    rotationSource = floatSource;
}

RotationTransformation::~RotationTransformation()
{
    if(rotationSource != null)
    {
        delete rotationSource;
    }
}

void RotationTransformation::Initialize(float _rotationDegrees, RotationDirection _direction)
{
    rotation = IW_ANGLE_FROM_DEGREES(_rotationDegrees);
    direction = _direction;
    rotationSource = null;
}

void RotationTransformation::ApplyTransformation(TransformationState& state)
{
    if(rotationSource != null)
    {
        rotation = IW_ANGLE_FROM_DEGREES(rotationSource->GetFloat(state.gameTime));
    }

    if(direction == CounterClockwise)
    {
        state.orientation = -rotation;
        state.tangent = state.orientation;
        state.normal = ANGLE_SUBTRACTION(state.orientation, ANGLE_90_DEGREES);
    }
    else
    {
        state.orientation = rotation;
        state.tangent = state.orientation;
        state.normal = ANGLE_ADDITION(state.orientation, ANGLE_90_DEGREES);
    }
}
