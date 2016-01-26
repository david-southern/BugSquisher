#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class BrickTarget;
class WallLevel;

class Brick : public Sprite
{
    std::string suffix;
    TextSprite *suffixSprite;
    WallLevel *parent;
    bool isSelected;
    int imageHandle;

public:
    Brick(GameManager *mgr, WallLevel *_parent, int _xPos);
    Brick(GameManager *mgr, WallLevel *_parent, int _xPos, int _yPos);

    void Stop();

    string GetSuffix();
    void TouchStart(uint16 x, uint16 y);
    void SetSelected(bool selected);
    void PlaceBrick(EventCallback *eventData);

    int xPos;
    BrickTarget *target;
    bool staticBrick;
};
