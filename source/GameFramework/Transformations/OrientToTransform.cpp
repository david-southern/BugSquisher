#include "s3e.h"
#include "Iw2D.h"

#include "OrientToTransform.h"

OrientToTransformation::OrientToTransformation(CIwVec2 location, OrientationDirection orientDirection)
{
    this->directional = null;
    this->locationSource = null;
    this->orientLocation = SCREEN_TO_SUBPIXEL(location);
    this->orientDirection = orientDirection;
}

OrientToTransformation::OrientToTransformation(IDirectionalSource *directional, OrientationDirection orientDirection)
{
    this->locationSource = null;
    this->directional = directional;
    this->orientDirection = orientDirection;
}

OrientToTransformation::OrientToTransformation(ILocationSource *locationSource, OrientationDirection orientDirection)
{
    this->directional = null;
    this->locationSource = locationSource;
    this->orientDirection = orientDirection;
}

OrientToTransformation::~OrientToTransformation()
{
    if(directional != null && directional->dirSourceNeedsDelete)
    {
        delete directional;
    }

    if(locationSource != null && locationSource->locSourceNeedsDelete)
    {
        delete locationSource;
    }
}


void OrientToTransformation::ApplyTransformation(TransformationState& state)
{
    iwangle rotAngle = 0, normal = 0;

    if(directional != null)
    {
        if(orientDirection == OrientAlongNormal || orientDirection == OrientReverseNormal || orientDirection == OrientAway || orientDirection == OrientToward)
        {
            rotAngle = directional->GetNormal(state.gameTime);
            normal = directional->GetTangent(state.gameTime);

            if(orientDirection == OrientReverseNormal || orientDirection == OrientToward)
            {
                rotAngle = ANGLE_ADDITION(rotAngle, ANGLE_180_DEGREES);
                normal = ANGLE_ADDITION(normal, ANGLE_180_DEGREES);
            }
        }
        else if(orientDirection == OrientAlongTangent || orientDirection == OrientReverseTangent)
        {
            rotAngle = directional->GetTangent(state.gameTime);
            normal = directional->GetNormal(state.gameTime);

            if(orientDirection == OrientReverseTangent)
            {
                rotAngle = ANGLE_ADDITION(rotAngle, ANGLE_180_DEGREES);
                normal = ANGLE_ADDITION(normal, ANGLE_180_DEGREES);
            }
        }
        else if(orientDirection == OrientAlongOrientation || orientDirection == OrientReverseOrientation)
        {
            rotAngle = directional->GetOrientation(state.gameTime);
            normal = directional->GetNormal(state.gameTime);

            if(orientDirection == OrientReverseOrientation)
            {
                rotAngle = ANGLE_ADDITION(rotAngle, ANGLE_180_DEGREES);
                normal = ANGLE_ADDITION(normal, ANGLE_180_DEGREES);
            }
        }
    }
    else
    {
        CIwVec2 targetLocation = orientLocation;

        if(locationSource != null)
        {
            targetLocation = locationSource->GetLocation(state.gameTime);
        }

        CIwVec2 orientVec = targetLocation - state.currentLocation;

        if(orientVec == CIwVec2::g_Zero)
        {
            rotAngle = 0;
        }
        else
        {
            rotAngle = IwGeomAtan2(orientVec.y, orientVec.x); 
        }

        normal = ANGLE_ADDITION(rotAngle, ANGLE_90_DEGREES);
        if(orientDirection == OrientAway)
        {
            rotAngle = ANGLE_ADDITION(rotAngle, ANGLE_180_DEGREES);
            normal = ANGLE_ADDITION(normal, ANGLE_180_DEGREES);
        }
    }
   
    state.orientation = rotAngle;
    state.tangent = rotAngle;
    state.normal = normal;
}
