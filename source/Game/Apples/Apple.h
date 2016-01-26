#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class Apple : public Sprite
{
    bool isBadApple;
    std::string prefix;
    TextSprite *prefixSprite;

    CIwVec2 GetNewLocation();
    void ReleaseLocation(CIwVec2 location);

public:
    Apple(GameManager *mgr);

    void Stop();
    bool IsValidWord();

    void AppleDrop(EventCallback *eventData);
    void AppleCollected(EventCallback *eventData);

    void TouchStart(uint16 x, uint16 y);
};
