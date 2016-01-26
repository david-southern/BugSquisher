#pragma once

#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "Sprite.h"

class SpriteGroup : public Sprite
{
    std::vector<Sprite*> *children;

    void RenderImage(uint32 gameTime);
public:
    SpriteGroup(GameManager *gameManager);
    ~SpriteGroup();

    void AddChild(Sprite *child);
};


