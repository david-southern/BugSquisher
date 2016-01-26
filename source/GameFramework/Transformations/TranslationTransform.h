#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

class TranslationTransformation : public ITransformation
{
    CIwVec2 offset;
    ILocationSource *offsetSource;

public:
    TranslationTransformation(CIwVec2 _offset);
    TranslationTransformation(ILocationSource *locationSource);

    ~TranslationTransformation();

    virtual void ApplyTransformation(TransformationState& state);
};


