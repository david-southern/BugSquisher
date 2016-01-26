#include "s3e.h"
#include "Iw2D.h"

#include "ScaleTransform.h"

ScaleTransformation::ScaleTransformation(float scaleFactor)
{
    this->scaleFactor = IW_FIXED(scaleFactor);
    scaleFactorSource = null;
}

ScaleTransformation::ScaleTransformation(IFloatSource *floatSource)
{
    scaleFactorSource = floatSource;
}

ScaleTransformation::~ScaleTransformation()
{
    if(scaleFactorSource != null)
    {
        delete scaleFactorSource;
    }
}

void ScaleTransformation::ApplyTransformation(TransformationState& state)
{
    if(scaleFactorSource != null)
    {
        scaleFactor = IW_FIXED(scaleFactorSource->GetFloat(state.gameTime));
    }

    state.scaleFactor = scaleFactor;
}
