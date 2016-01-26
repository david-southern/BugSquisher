#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

class WallLevel;

class BrickTarget : public Sprite
{
    std::string prefix;
    TextSprite *prefixSprite;
    bool isSelected;
    WallLevel *parent;

public:
    BrickTarget(GameManager *mgr, WallLevel *_parent, int _xPos, int _yPos);
    
    string GetPrefix();
    void TouchStart(uint16 x, uint16 y);
    void SetSelected(bool selected);
    void Stop();
    void Hide();

    int xPos, yPos;
    bool staticTarget;
};
