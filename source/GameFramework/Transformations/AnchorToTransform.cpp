#include "s3e.h"
#include "Iw2D.h"

#include "AnchorToTransform.h"

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, CIwVec2 _offset)
{
    Initialize();
    locSource = _locSource;
    offset = SCREEN_TO_SUBPIXEL(_offset);
}

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, ILocationSource *_offsetSource)
{
    Initialize();
    locSource = locSource;
    offsetSource = _offsetSource;
}

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource,
    float _radius, float _orientationOffsetDegrees, AnchorType _anchorType)
{
    Initialize();
    locSource = _locSource;
    orientationSource = _orientationSource;
    anchorType = _anchorType;
    orientationOffset = IW_ANGLE_FROM_DEGREES(_orientationOffsetDegrees);
    radius = IW_FIXED(_radius);

    if(anchorType == RelativeToAnchorPoint)
    {
        CalculateOffset(orientationOffset);
    }
}

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource,
    IFloatSource *_radiusSource, float _orientationOffsetDegrees, AnchorType _anchorType)
{
    Initialize();
    locSource = _locSource;
    orientationSource = _orientationSource;
    anchorType = _anchorType;
    radiusSource = _radiusSource;
    orientationOffset = IW_ANGLE_FROM_DEGREES(_orientationOffsetDegrees);

    if(anchorType == RelativeToAnchorPoint)
    {
        CalculateOffset(orientationOffset);
    }
}

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource,
    float _radius, IFloatSource *_orientationOffsetDegreesSource, AnchorType _anchorType)
{
    Initialize();
    locSource = _locSource;
    orientationSource = _orientationSource;
    anchorType = _anchorType;
    radius = IW_FIXED(_radius);
    orientationOffsetSource = _orientationOffsetDegreesSource;
}

AnchorToTransformation::AnchorToTransformation(ILocationSource *_locSource, IDirectionalSource *_orientationSource,
    IFloatSource *_radiusSource, IFloatSource *_orientationOffsetDegreesSource, AnchorType _anchorType)
{
    Initialize();
    locSource = _locSource;
    orientationSource = _orientationSource;
    anchorType = _anchorType;
    radiusSource = _radiusSource;
    orientationOffsetSource = _orientationOffsetDegreesSource;
}

AnchorToTransformation::~AnchorToTransformation()
{
    if(locSource == null && locSource->locSourceNeedsDelete)
    {
        delete locSource;
    }

    if(offsetSource == null && offsetSource->locSourceNeedsDelete)
    {
        delete offsetSource;
    }

    if(orientationSource == null && orientationSource->dirSourceNeedsDelete)
    {
        delete orientationSource;
    }

    if(radiusSource == null)
    {
        delete radiusSource;
    }

    if(orientationOffsetSource == null)
    {
        delete orientationOffsetSource;
    }

}

void AnchorToTransformation::Initialize()
{
    locSource = null;
    offset = CIwVec2(0, 0);
    anchorType = RelativeToAnchorPoint;
    offsetSource = null;
    orientationSource = null;
    radius = IW_FIXED(0);
    radiusSource = null;
    orientationOffset = IW_ANGLE_FROM_DEGREES(0);
    orientationOffsetSource = null;
}

void AnchorToTransformation::CalculateOffset(iwangle orientation)
{
    offset.x = IW_FIXED_MUL(IwGeomCos(orientation), radius) >> IW_GEOM_POINT;
    offset.y = IW_FIXED_MUL(IwGeomSin(orientation), radius) >> IW_GEOM_POINT;
    offset = SCREEN_TO_SUBPIXEL(offset);
}

void AnchorToTransformation::ApplyTransformation(TransformationState& state)
{
    if(radiusSource != null)
    {
        radius = IW_FIXED(radiusSource->GetFloat(state.gameTime));
    }

    if(orientationOffsetSource != null)
    {
        orientationOffset = IW_ANGLE_FROM_DEGREES(orientationOffsetSource->GetFloat(state.gameTime));
        if(anchorType == RelativeToAnchorPoint)
        {
            CalculateOffset(orientationOffset);
        }
    }

    if(anchorType == RelativeToOrientation)
    {
        iwangle orientation = orientationSource->GetOrientation(state.gameTime);
        orientation = ANGLE_ADDITION(orientation, orientationOffset);

        CalculateOffset(orientation);
    }

    if(anchorType == RelativeToNormal)
    {
        iwangle orientation = orientationSource->GetNormal(state.gameTime);
        orientation = ANGLE_ADDITION(orientation, orientationOffset);

        CalculateOffset(orientation);
    }

    if(anchorType == RelativeToTangent)
    {
        iwangle orientation = orientationSource->GetTangent(state.gameTime);
        orientation = ANGLE_ADDITION(orientation, orientationOffset);

        CalculateOffset(orientation);
    }

    state.currentLocation = locSource->GetLocation(state.gameTime) + offset;
}
