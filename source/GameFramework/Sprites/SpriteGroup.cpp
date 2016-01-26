#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "SpriteGroup.h"

SpriteGroup::SpriteGroup(GameManager *gameManager) : Sprite(gameManager)
{
    children = new std::vector<Sprite*>;
}

SpriteGroup::~SpriteGroup()
{
    delete children;
}

void SpriteGroup::AddChild(Sprite *child)
{
    if(child == this)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Cannot add a SpriteGroup to itself");
        return;
    }

    for(uint32 childIndex = 0; childIndex < children->size(); childIndex++)
    {
        if(children->at(childIndex) == child)
        {
            s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "SpriteGroup: Child added multiple times");
            return;
        }
    }

    children->push_back(child);
    child->SetParent(this);
}

void SpriteGroup::RenderImage(uint32 gameTime)
{
}