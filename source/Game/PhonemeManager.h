#pragma once

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class PhonemeManager : public TextSprite
{
    const char *currentPhoneme;
    bool doPrefixes;

public:
    PhonemeManager(GameManager *mgr, bool _doPrefixes);

    void ChangePhoneme();
    const char *GetPhoneme();
};
