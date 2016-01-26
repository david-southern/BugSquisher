#include "s3e.h"
#include "Iw2D.h"

#include "TranslationTransform.h"

TranslationTransformation::TranslationTransformation(CIwVec2 _offset)
{
    offset = SCREEN_TO_SUBPIXEL(_offset);
    offsetSource = null;
}

TranslationTransformation::TranslationTransformation(ILocationSource *locationSource)
{
    offsetSource = locationSource;
}

TranslationTransformation::~TranslationTransformation()
{
    if(offsetSource != null)
    {
        delete offsetSource;
    }
}

void TranslationTransformation::ApplyTransformation(TransformationState& state)
{
    if(offsetSource != null && offsetSource->locSourceNeedsDelete)
    {
        offset = SCREEN_TO_SUBPIXEL(offsetSource->GetLocation(state.gameTime));
    }

    state.currentLocation += offset;
}
