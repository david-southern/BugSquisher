#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class Dumptruck : public PathFollowingSprite
{
    std::string prefix;
    TextSprite *prefixSprite;

    void BeginDump(EventCallback *eventData);
    void EndDump(EventCallback *eventData);

public:
    Dumptruck(GameManager *mgr);

    void Stop();
    bool IsValidWord();

    void TouchStart(uint16 x, uint16 y);
};
