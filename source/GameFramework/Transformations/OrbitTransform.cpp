#include "s3e.h"
#include "Iw2D.h"

#include "IwGeomCore.h"

#include "OrbitTransform.h"

OrbitTransformation::OrbitTransformation(int32 radius, uint32 orbitTimeMillis, float initialAngleDegrees, RotationDirection rotationDirection, OrientationDirection orientationDirection) : IDirectionalSource()
{
    if(orbitTimeMillis > MAX_ORBIT_TIME_MILLIS)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "OrbitTransformation may not have an orbitTimeMillis > 300000");
        orbitTimeMillis = MAX_ORBIT_TIME_MILLIS;
    }

    // Shift for sub-pixel rendering
    this->radius = SCREEN_TO_SUBPIXEL(radius);
    this->initialAngle = IW_ANGLE_FROM_DEGREES(initialAngleDegrees);
    this->orbitTimeMillis = orbitTimeMillis;
    this->rotationDirection = rotationDirection;
    this->orientationDirection = orientationDirection;
    radiusSource = null;

    birthdayMillis = 0; 
}

OrbitTransformation::OrbitTransformation(IIntSource *_radiusSource, uint32 orbitTimeMillis, float initialAngleDegrees, RotationDirection rotationDirection, OrientationDirection orientationDirection) : IDirectionalSource()
{
    if(orbitTimeMillis > MAX_ORBIT_TIME_MILLIS)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "OrbitTransformation may not have an orbitTimeMillis > 300000");
        orbitTimeMillis = MAX_ORBIT_TIME_MILLIS;
    }

    this->initialAngle = IW_ANGLE_FROM_DEGREES(initialAngleDegrees);
    this->orbitTimeMillis = orbitTimeMillis;
    this->rotationDirection = rotationDirection;
    this->orientationDirection = orientationDirection;

    radiusSource = _radiusSource;

    birthdayMillis = 0; 
}

OrbitTransformation::~OrbitTransformation()
{
    if(radiusSource != null)
    {
        delete radiusSource;
    }
}

iwangle OrbitTransformation::GetAngle(uint32 gameTime)
{
    if(birthdayMillis == 0)
    {
        birthdayMillis = gameTime;
    }

    int32 age = gameTime - birthdayMillis;
    age = age % orbitTimeMillis;

    if(rotationDirection == CounterClockwise)
    {
        age = -age;        
    }

    return ANGLE_ADDITION(IW_FIXED_MUL_SAFE(IW_FIXED_DIV_SAFE(IW_FIXED(age), IW_FIXED(orbitTimeMillis)), IW_ANGLE_2PI), initialAngle);
}

void OrbitTransformation::ApplyTransformation(TransformationState& state)
{
    iwangle orbitAngle = GetAngle(state.gameTime);

    CIwVec2 orbitNormal(IwGeomCos(orbitAngle), IwGeomSin(orbitAngle));

    if(radiusSource != null)
    {
        radius = SCREEN_TO_SUBPIXEL(radiusSource->GetInt(state.gameTime));
    }

    orbitNormal *= radius;
    state.currentLocation += orbitNormal;

    if(orientationDirection != None)
    {
        state.orientation = GetOrientation(state.gameTime);
        state.tangent = GetTangent(state.gameTime);
        state.normal = GetNormal(state.gameTime);
    }
}

iwangle OrbitTransformation::GetNormal(uint32 gameTime)
{
    return GetAngle(gameTime);
}

iwangle OrbitTransformation::GetTangent(uint32 gameTime)
{
    iwangle normal = GetAngle(gameTime);
    iwangle tanAngle;

    if(rotationDirection == Clockwise)
    {
        tanAngle = ANGLE_ADDITION(normal, ANGLE_90_DEGREES);
    }
    else
    {
        tanAngle = ANGLE_SUBTRACTION(normal, ANGLE_90_DEGREES);
    }

    return tanAngle;
}

iwangle OrbitTransformation::GetOrientation(uint32 gameTime)
{
    iwangle orbitAngle = GetAngle(gameTime);
    iwangle tangent;
    iwangle orientation = 0;

    if(rotationDirection == Clockwise)
    {
        tangent = IW_ANGLE_MASK_PHASE(orbitAngle + ANGLE_90_DEGREES);
    }
    else
    {
        tangent = IW_ANGLE_MASK_PHASE(orbitAngle - ANGLE_90_DEGREES);
    }

    switch(orientationDirection)
    {
    case OrientAlongTangent:
        orientation = tangent;
        break;

    case OrientReverseTangent:
        orientation = ANGLE_ADDITION(tangent, ANGLE_180_DEGREES);
        break;

    case OrientAway:
    case OrientAlongNormal:
        orientation = orbitAngle;
        break;

    case OrientToward:
    case OrientReverseNormal:
        orientation = ANGLE_ADDITION(orbitAngle, ANGLE_180_DEGREES);
        break;

    default:
        orientation = 0;
        break;
    }

    return orientation;
}
