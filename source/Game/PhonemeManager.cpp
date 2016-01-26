#include "GameVariables.h"
#include "PhonemeManager.h"

PhonemeManager::PhonemeManager(GameManager *mgr, bool _doPrefixes) : TextSprite(mgr, "")
{
    doPrefixes = _doPrefixes;

    mgr->RegisterGameObject(this);

    AddTransformation(new AnchorTransformation());
    SetBaseLocation(CIwVec2(110, 110));
    SetFont(mgr->GetResourceManager()->GetFont(FONT_COOPER_48));
    SetZIndex(8);

    strcpy(this->debugId, "PhonemeManager");

    ChangePhoneme();
}

void PhonemeManager::ChangePhoneme()
{
    if(doPrefixes)
    {
        currentPhoneme = wordBuilder->GetNextPrefix();
    }
    else
    {
        currentPhoneme = wordBuilder->GetNextSuffix();
    }

    SetText(currentPhoneme);
    StateVariable newPhoneme;
    newPhoneme.stringValue = currentPhoneme;
    mgr->GetStateManager()->SetVar(VAR_CURRENT_PHONEME, newPhoneme);
}

const char *PhonemeManager::GetPhoneme()
{
    return currentPhoneme;
}
