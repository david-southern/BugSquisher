#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class Bug : public FlipbookSprite
{
    std::string suffix;
    TextSprite *suffixSprite;
    PathFollowingSprite *pathSprite;

    CIwFVec2 startPoint, CP1, CP2, endPoint;
    CIwFVec2 GetRandomPoint(int buffer);
    void GetNextPath(bool lastPath = false);
    CIwFVec2 ConstrainToScreen(CIwFVec2 startPoint, CIwFVec2 normal, float padding);

public:
    Bug(GameManager *mgr);

    void Stop();
    bool IsValidWord();
    void BugFinished(EventCallback *eventData);

    void TouchStart(uint16 x, uint16 y);
};
